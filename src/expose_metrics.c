#include "../include/expose_metrics.h"
#include "metrics.c"

/** Mutex para sincronización de hilos */
pthread_mutex_t lock;

/** Métrica de Prometheus para el uso de CPU */
static prom_gauge_t* cpu_usage_metric;

/** Métrica de Prometheus para el uso de memoria */
static prom_gauge_t* memory_usage_metric;

/** Métrica de Prometheus para el uso del disco */
static prom_gauge_t* disk_usage_metric;

/** Métrica de Prometheus para los bytes recibidos */
static prom_gauge_t* net_rx_packages_metric;

/** Métrica de Prometheus para los bytes transmitidos */
static prom_gauge_t* net_tx_packages_metric;

/** Métrica de Prometheus para los procesos en ejecucion */
static prom_gauge_t* procs_usage_metric;

/** Métrica de Prometheus para los procesos en ejecucion */
static prom_gauge_t* ctxt_usage_metric;

void update_cpu_gauge()
{
    double usage = get_cpu_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(cpu_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
        // printf("Actualizando métrica de CPU: %f\n", usage);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de CPU\n");
    }
}

void update_memory_gauge()
{
    double usage = get_memory_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(memory_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
        // printf("Actualizando métrica de memoria: %f\n", usage);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de memoria\n");
    }
}

void update_disk_gauge(const char* disk_name)
{
    double usage = get_disk_usage(disk_name);
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(disk_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
        // printf("Actualizando métrica de disco: %f\n", usage);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de disco\n");
    }
}

void update_net_gauge(const char* interface_name)
{
    NetUsage net_usage = get_net_usage(interface_name);
    if (net_usage.rx_packages >= 0 && net_usage.tx_packages >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(net_rx_packages_metric, net_usage.rx_packages, NULL); // Métrica para paquetes recibidos
        prom_gauge_set(net_tx_packages_metric, net_usage.tx_packages, NULL); // Métrica para paquetes transmitidos
        pthread_mutex_unlock(&lock);
    }
    else
    {
        fprintf(stderr, "Error al obtener el uso de red\n");
    }
}

void update_procs_gauge()
{
    int procs_usage = get_process_usage();
    if (procs_usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(procs_usage_metric, procs_usage, NULL);
        pthread_mutex_unlock(&lock);
        // printf("Actualizando métrica de procesos: %d\n", procs_usage);
    }
    else
    {
        fprintf(stderr, "Error al obtener el numero de procesos\n");
    }
}

void update_ctxt_gauge()
{
    double usage = get_ctxt_usage();
    if (usage >= 0)
    {
        pthread_mutex_lock(&lock);
        prom_gauge_set(ctxt_usage_metric, usage, NULL);
        pthread_mutex_unlock(&lock);
        // printf("Actualizando métrica de cambios de contextos: %f\n", usage);
    }
    else
    {
        fprintf(stderr, "Error al obtener el numero de cambios de contexto\n");
    }
}

void* expose_metrics(void* arg)
{
    (void)arg; // Argumento no utilizado

    // Aseguramos que el manejador HTTP esté adjunto al registro por defecto
    promhttp_set_active_collector_registry(NULL);

    // Iniciamos el servidor HTTP en el puerto 8000
    struct MHD_Daemon* daemon = promhttp_start_daemon(MHD_USE_SELECT_INTERNALLY, 8000, NULL, NULL);
    if (daemon == NULL)
    {
        fprintf(stderr, "Error al iniciar el servidor HTTP\n");
        return NULL;
    }

    // Mantenemos el servidor en ejecución
    while (1)
    {
        sleep(1);
    }

    // Nunca debería llegar aquí
    MHD_stop_daemon(daemon);
    return NULL;
}

void init_metrics()
{
    // Inicializamos el mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        fprintf(stderr, "Error al inicializar el mutex\n");
    }

    // Inicializamos el registro de coleccionistas de Prometheus
    if (prom_collector_registry_default_init() != 0)
    {
        fprintf(stderr, "Error al inicializar el registro de Prometheus\n");
    }

    // Creamos la métrica para el uso de CPU
    cpu_usage_metric = prom_gauge_new("cpu_usage_percentage", "Porcentaje de uso de CPU", 0, NULL);
    if (cpu_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de CPU\n");
    }

    // Creamos la métrica para el uso de memoria
    memory_usage_metric = prom_gauge_new("memory_usage_percentage", "Porcentaje de uso de memoria", 0, NULL);
    if (memory_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso de memoria\n");
    }

    // Creamos la métrica para el uso del disco
    disk_usage_metric = prom_gauge_new("disk_usage_percentage", "Porcentaje de uso del disco", 0, NULL);
    if (disk_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de uso del disco\n");
    }

    // Creamos la métrica para los bytes recibidos de la red
    net_rx_packages_metric = prom_gauge_new("net_rx_packages", "Paquetes recibidos por la interfaz de red", 0, NULL);
    if (net_rx_packages_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de paquetes recibidos de la red\n");
    }

    // Creamos la métrica para los paquetes transmitidos de la red
    net_tx_packages_metric = prom_gauge_new("net_tx_packages", "Paquetes transmitidos por la interfaz de red", 0, NULL);
    if (net_tx_packages_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de paquetes transmitidos de la red\n");
    }

    // Creamos la métrica para el uso de los procesos en ejecucion
    procs_usage_metric = prom_gauge_new("procs_usage_count", "Cantidad de procesos en ejecucion", 0, NULL);
    if (procs_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cantidad de procesos\n");
    }

    // Creamos la métrica para el numero de cambios de contextos desde que inicio el sistema
    ctxt_usage_metric = prom_gauge_new("ctxt_usage_count", "Cantidad de cambios de contexto", 0, NULL);
    if (ctxt_usage_metric == NULL)
    {
        fprintf(stderr, "Error al crear la métrica de cantidad de cambios de contextos\n");
    }

    // Registramos las métricas en el registro por defecto
    if (prom_collector_registry_must_register_metric(memory_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - memoria\n");
    }
    if (prom_collector_registry_must_register_metric(cpu_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - cpu\n");
    }
    if (prom_collector_registry_must_register_metric(disk_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - disk\n");
    }
    if (prom_collector_registry_must_register_metric(net_rx_packages_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - paquetes recibidos\n");
    }
    if (prom_collector_registry_must_register_metric(net_tx_packages_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - paquetes transmitidos\n");
    }
    if (prom_collector_registry_must_register_metric(procs_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - procesos\n");
    }
    if (prom_collector_registry_must_register_metric(ctxt_usage_metric) == NULL)
    {
        fprintf(stderr, "Error al registrar las métricas - cambios de contexto\n");
    }
}

void destroy_mutex()
{
    pthread_mutex_destroy(&lock);
}
