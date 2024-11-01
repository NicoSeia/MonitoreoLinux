#include "../include/config.h"
#include "../include/metrics.h"

MetricsConfig read_metrics_config(const char* config_file)
{
    MetricsConfig config = {0, 0, 0, 0, 0, 0};
    FILE* file = fopen(config_file, "r");
    if (!file)
    {
        fprintf(stderr, "Error al abrir el archivo de configuración: %s\n", config_file);
        return config;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* data = malloc(length + 1);
    fread(data, 1, length, file);
    fclose(file);
    data[length] = '\0';

    cJSON* json = cJSON_Parse(data);
    if (json)
    {
        // printf("Archivo JSON parseado con éxito.\n");
        cJSON* metrics = cJSON_GetObjectItem(json, "metrics");
        if (metrics)
        {
            config.cpu = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "cpu"));
            config.memory = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "memory"));
            config.disk = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "disk"));
            config.network = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "network"));
            config.processes = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "processes"));
            config.context_switches = cJSON_IsTrue(cJSON_GetObjectItem(metrics, "context_switches"));
        }
        else
        {
            printf("Objeto 'metrics' no encontrado en JSON.\n");
        }
        cJSON_Delete(json);
    }
    else
    {
        printf("Error al parsear el archivo JSON.\n");
    }

    free(data);
    return config;
}

char* create_metrics_json(MetricsConfig config)
{
    // Crear un objeto cJSON para las métricas
    cJSON* json = cJSON_CreateObject();

    if (config.cpu)
    {
        double cpu_usage = get_cpu_usage();
        cJSON_AddNumberToObject(json, "cpu_usage", cpu_usage);
    }

    if (config.memory)
    {
        double memory_usage = get_memory_usage();
        cJSON_AddNumberToObject(json, "memory_usage", memory_usage);
    }

    if (config.disk)
    {
        double disk_usage = get_disk_usage();
        cJSON_AddNumberToObject(json, "disk_usage", disk_usage);
    }

    if (config.network)
    {
        double network_usage = get_network_usage("lo");
        cJSON_AddNumberToObject(json, "network_usage", network_usage);
    }

    if (config.processes)
    {
        int process_count = get_process_usage();
        cJSON_AddNumberToObject(json, "process_count", process_count);
    }

    if (config.context_switches)
    {
        double ctxt_usage = get_ctxt_usage();
        cJSON_AddNumberToObject(json, "context_switches", ctxt_usage);
    }

    // Convertir el objeto cJSON a una cadena
    char* json_string = cJSON_Print(json);
    if (json_string != NULL)
    {
        printf("JSON de métricas creado con éxito:\n%s\n", json_string);
    }
    cJSON_Delete(json);

    return json_string;
}

void send_metrics_to_monitor()
{
    // Obtener la ruta actual
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        // Construir la ruta absoluta al archivo de configuración
        char config_file_path[1024];
        snprintf(config_file_path, sizeof(config_file_path), "%s/../config.json", cwd);

        // Leer la configuración directamente en esta función
        MetricsConfig config = read_metrics_config(config_file_path);

        char* json_string = create_metrics_json(config);

        if (json_string != NULL)
        {
            // Abrir el FIFO en modo escritura
            int fifo_fd = open("/tmp/monitor_fifo", O_WRONLY | O_NONBLOCK);
            if (fifo_fd != -1)
            {
                // Escribir los datos en el FIFO y cerrar el descriptor de archivo
                ssize_t bytes_written = write(fifo_fd, json_string, strlen(json_string));
                if (bytes_written == -1)
                {
                    perror("Error al escribir en el pipe");
                }
                else
                {
                    printf("Métricas enviadas a través del pipe.\n");
                }
                close(fifo_fd);
            }
            else
            {
                perror("Error al abrir el pipe para enviar métricas");
            }

            // Liberar la cadena JSON
            free(json_string);
        }
        else
        {
            printf("Error: No se pudo crear el JSON de métricas.\n");
        }
    }
    else
    {
        perror("Error al obtener el directorio actual");
    }
}
