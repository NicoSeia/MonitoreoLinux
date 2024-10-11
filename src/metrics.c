#include "../include/metrics.h"

/**
 * @brief Tamaño del buffer utilizado para almacenar el nombre del disco.
 */
#define SIZE_BUFF 32

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

double get_disk_usage()
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long reads_completed = 0, writes_completed = 0;
    double usage = 0;

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
        char device_name[SIZE_BUFF];

        // Escanear el nombre del dispositivo y los campos necesarios
        int ret =
            sscanf(buffer, "%*d %*d %31s %llu %*s %*s %*s %llu", device_name, &reads_completed, &writes_completed);

        // Comparar el nombre del dispositivo con el nombre del disco pasado
        if (ret == 5 && strcmp(device_name, "sdb") == 0)
        {
            break; // Si encontramos una coincidencia, salimos del bucle
        }
    }

    fclose(fp);

    // Verificar si se encontraron estadísticas del disco
    if (reads_completed == 0 && writes_completed == 0)
    {
        fprintf(stderr, "Error: No se encontraron estadísticas para el disco sdb\n");
        return usage;
    }

    // Calculo total de escrituras y lecturas
    double total = reads_completed + writes_completed;

    return total;
}

double get_network_usage(const char* interface)
{
    FILE* fp;
    char buffer[BUFFER_SIZE];
    unsigned long long int rx_bytes = 0, tx_bytes = 0;

    // Abrir el archivo /proc/net/dev
    fp = fopen("/proc/net/dev", "r");
    if (fp == NULL)
    {
        perror("Error al abrir /proc/net/dev");
        return -1.0;
    }

    // Saltar las dos primeras líneas
    fgets(buffer, sizeof(buffer), fp);
    fgets(buffer, sizeof(buffer), fp);

    while (fgets(buffer, sizeof(buffer), fp) != NULL)
    {
        char iface[32];
        // Leer el nombre de la interfaz
        sscanf(buffer, "%31s", iface); // Limitar a 31 caracteres para evitar desbordamiento

        if (strncmp(iface, interface, strlen(interface)) == 0)
        {
            // Cambiar el formato para que coincida con las variables
            sscanf(buffer, "%*s %llu %*d %*d %*d %*d %*d %*d %llu", &rx_bytes, &tx_bytes);
            break;
        }
    }

    fclose(fp);

    double total_bytes = (double)(rx_bytes + tx_bytes);

    return total_bytes;
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
