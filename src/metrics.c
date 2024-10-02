#include "../include/metrics.h"

/**
 * @struct NetUsage
 * @brief Estructura que almacena información sobre el uso de red de una interfaz específica.
 *
 * Esta estructura contiene el nombre de la interfaz de red y los paquetes recibidos y transmitidos
 * por dicha interfaz. Es utilizada para obtener métricas de red desde el archivo `/proc/net/dev`.
 */
typedef struct
{
    /**
     * @brief Nombre de la interfaz de red.
     *
     * Este campo contiene el nombre de la interfaz de red (como "eth0", "wlan0", etc.).
     * Se almacena un máximo de 31 caracteres más el terminador nulo.
     */
    char interface_name[32];

    /**
     * @brief Número de paquetes recibidos.
     *
     * Este campo almacena la cantidad total de paquetes recibidos por la interfaz de red
     * desde que se inició el sistema.
     */
    unsigned long long rx_packages; // Paquetes recibidos

    /**
     * @brief Número de paquetes transmitidos.
     *
     * Este campo almacena la cantidad total de paquetes transmitidos por la interfaz de red
     * desde que se inició el sistema.
     */
    unsigned long long tx_packages; // Paquetes transmitidos
} NetUsage;

double get_memory_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long total_mem = 0, free_mem = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/meminfo", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/meminfo");
        return -1.0;
    }

    // Leer los valores de memoria total y disponible
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (sscanf(buffer, "MemTotal: %llu kB", &total_mem) == 1)
        {
            continue; // MemTotal encontrado
        }
        if (sscanf(buffer, "MemAvailable: %llu kB", &free_mem) == 1)
        {
            break; // MemAvailable encontrado, podemos dejar de leer
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_mem == 0 || free_mem == 0)
    {
        fprintf(stderr, "Error al leer la información de memoria desde /proc/meminfo\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de memoria
    double used_mem = total_mem - free_mem;
    double mem_usage_percent = (used_mem / total_mem) * 100.0;

    return mem_usage_percent;
}

double get_cpu_usage()
{
    static unsigned long long prev_user = 0, prev_nice = 0, prev_system = 0, prev_idle = 0, prev_iowait = 0,
                              prev_irq = 0, prev_softirq = 0, prev_steal = 0;
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
    unsigned long long totald, idled;
    double cpu_usage_percent;

    // Abrir el archivo /proc/stat
    FILE* fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    char buffer[BUFFER_SIZE * 4];
    if (fgets(buffer, sizeof(buffer), fp) == NULL)
    {
        perror("Error al leer /proc/stat");
        fclose(fp);
        return -1.0;
    }
    fclose(fp);

    // Analizar los valores de tiempo de CPU
    int ret = sscanf(buffer, "cpu  %llu %llu %llu %llu %llu %llu %llu %llu", &user, &nice, &system, &idle, &iowait,
                     &irq, &softirq, &steal);
    if (ret < 8)
    {
        fprintf(stderr, "Error al parsear /proc/stat\n");
        return -1.0;
    }

    // Calcular las diferencias entre las lecturas actuales y anteriores
    unsigned long long prev_idle_total = prev_idle + prev_iowait;
    unsigned long long idle_total = idle + iowait;

    unsigned long long prev_non_idle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;
    unsigned long long non_idle = user + nice + system + irq + softirq + steal;

    unsigned long long prev_total = prev_idle_total + prev_non_idle;
    unsigned long long total = idle_total + non_idle;

    totald = total - prev_total;
    idled = idle_total - prev_idle_total;

    if (totald == 0)
    {
        fprintf(stderr, "Totald es cero, no se puede calcular el uso de CPU!\n");
        return -1.0;
    }

    // Calcular el porcentaje de uso de CPU
    cpu_usage_percent = ((double)(totald - idled) / totald) * 100.0;

    // Actualizar los valores anteriores para la siguiente lectura
    prev_user = user;
    prev_nice = nice;
    prev_system = system;
    prev_idle = idle;
    prev_iowait = iowait;
    prev_irq = irq;
    prev_softirq = softirq;
    prev_steal = steal;

    return cpu_usage_percent;
}

double get_disk_usage(const char* disk_name)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long reads_completed = 0, writes_completed = 0;
    unsigned long long read_time = 0, write_time = 0;
    unsigned long long time_weighted_io = 0;

    // Abrir el archivo /proc/diskstats
    fp = fopen("/proc/diskstats", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/diskstats");
        return -1.0;
    }

    // Leer y procesar todas las líneas del archivo /proc/diskstats
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        char device_name[32];

        // Escanear el nombre del dispositivo y los campos necesarios
        int ret = sscanf(buffer, "%*d %*d %31s %llu %*s %*s %llu %llu %*s %*s %llu %*s %*s %llu %*s %*s %*s %*s %*s",
                         device_name, &reads_completed, &read_time, &writes_completed, &write_time, &time_weighted_io);

        // Comparar el nombre del dispositivo con el nombre del disco pasado
        if (ret == 5 && strcmp(device_name, disk_name) == 0)
        {
            break; // Si encontramos una coincidencia, salimos del bucle
        }
    }

    fclose(fp);

    // Verificar si se encontraron estadísticas del disco
    if (reads_completed == 0 && writes_completed == 0)
    {
        fprintf(stderr, "Error: No se encontraron estadísticas para el disco %s\n", disk_name);
        return -1.0;
    }

    // Imprimir los valores capturados
    // printf("Lecturas completadas: %llu\n", reads_completed);
    // printf("Tiempo de lectura: %llu ms\n", read_time);
    // printf("Escrituras completadas: %llu\n", writes_completed);
    // printf("Tiempo de escritura: %llu ms\n", write_time);
    // printf("Tiempo ponderado de IO: %llu ms\n", time_weighted_io);

    // Calcular el porcentaje de uso del disco
    unsigned long long total_io_time = read_time + write_time;
    double disk_usage_percent = 0.0;

    // Verificar si el tiempo ponderado de IO es mayor que cero antes de calcular el porcentaje
    if (time_weighted_io > 0)
    {
        disk_usage_percent = (double)total_io_time / time_weighted_io * 100.0;
    }

    return disk_usage_percent;
}

NetUsage get_net_usage(const char* interface_name)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    NetUsage net_usage = {0}; // Inicializar estructura a cero

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return net_usage; // Devuelve estructura vacía en caso de error
    }

    // Leer líneas y procesar
    // Saltar las dos primeras líneas (cabeceras)
    fgets(buffer, sizeof(buffer), fp);
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        // Escanear el nombre de la interfaz y los bytes recibidos y transmitidos
        char iface[32];
        unsigned long long rx_packages, tx_packages;

        // Aquí cambiamos el formato de scanf para incluir un tratamiento de espacios
        int ret =
            sscanf(buffer, " %31[^:]: %*u %llu %*u %*u %*u %*u %*u %*u %*u %llu", iface, &rx_packages, &tx_packages);

        // printf("ret: %d, iface: '%s', rx_bytes: %llu, tx_bytes: %llu\n", ret, iface, rx_packages, tx_packages);

        // Comparar el nombre de la interfaz con el pasado
        if (ret == 3 && strcmp(iface, interface_name) == 0)
        {
            // Asignar valores a la estructura
            strncpy(net_usage.interface_name, iface, sizeof(net_usage.interface_name) - 1);
            net_usage.rx_packages = rx_packages;
            net_usage.tx_packages = tx_packages;
            break; // Salir del bucle si encontramos la interfaz
        }
    }

    if (net_usage.rx_packages == 0 && net_usage.tx_packages == 0)
    {
        printf("No se encontró la interfaz %s o no hay datos.\n", interface_name);
    }
    else
    {
        // printf("Interfaz: %s\n", net_usage.interface_name);
        // printf("Paquetes recibidos: %llu\n", net_usage.rx_packages);
        // printf("Paquetes transmitidos: %llu\n", net_usage.tx_packages);
    }

    fclose(fp);
    return net_usage; // Retorna la estructura con los valores encontrados
}

int get_process_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned int total_process = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    // Leer los valores de procesos en ejecucion
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (strncmp(buffer, "procs_running", 13) == 0)
        {
            // Extraer el número de procesos en ejecución
            sscanf(buffer, "procs_running %u", &total_process);
            break; // Salir del bucle ya que hemos encontrado lo que buscamos
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (total_process == 0)
    {
        fprintf(stderr, "Error al leer la información de procesos desde /proc/stat\n");
        return -1.0;
    }

    return total_process;
}

double get_ctxt_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long ctxt = 0;

    // Abrir el archivo /proc/meminfo
    fp = fopen("/proc/stat", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/stat");
        return -1.0;
    }

    // Leer los valores de cambios de contextos desde que se inicio el sistema
    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        if (strncmp(buffer, "ctxt", 4) == 0)
        {
            // Extraer el número de cambios de contextos
            sscanf(buffer, "ctxt %lu", &ctxt);
            break; // Salir del bucle ya que hemos encontrado lo que buscamos
        }
    }

    fclose(fp);

    // Verificar si se encontraron ambos valores
    if (ctxt == 0)
    {
        fprintf(stderr, "Error al leer la información de cambios de contextos desde /proc/stat\n");
        return -1.0;
    }

    return (double)ctxt;
}
