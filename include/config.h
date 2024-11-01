#include <cjson/cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * @struct MetricsConfig
 * @brief Estructura para almacenar las métricas que se desean monitorear.
 *
 * Esta estructura permite habilitar o deshabilitar el monitoreo de cada métrica específica.
 */
typedef struct
{
    int cpu;              /**< Estado del monitoreo de CPU: 1 habilitado, 0 deshabilitado. */
    int memory;           /**< Estado del monitoreo de memoria: 1 habilitado, 0 deshabilitado. */
    int disk;             /**< Estado del monitoreo de disco: 1 habilitado, 0 deshabilitado. */
    int network;          /**< Estado del monitoreo de red: 1 habilitado, 0 deshabilitado. */
    int processes;        /**< Estado del monitoreo del número de procesos: 1 habilitado, 0 deshabilitado. */
    int context_switches; /**< Estado del monitoreo de cambios de contexto: 1 habilitado, 0 deshabilitado. */
} MetricsConfig;

/**
 * @brief Lee la configuración de métricas desde un archivo JSON.
 *
 * Esta función abre un archivo JSON especificado, lo lee completamente, y parsea el contenido
 * para extraer la configuración de métricas. Las métricas disponibles incluyen CPU, memoria,
 * disco, red, número de procesos e interrupciones de contexto.
 *
 * @param config_file Ruta del archivo JSON de configuración.
 * @return MetricsConfig Estructura que contiene los valores de configuración para cada métrica.
 *         Si hay un error al abrir o parsear el archivo, devuelve una estructura con todas las métricas deshabilitadas.
 */
MetricsConfig read_metrics_config(const char* config_file);

/**
 * @brief Crea una cadena JSON con las métricas seleccionadas en la configuración.
 *
 * A partir de una estructura `MetricsConfig`, esta función genera un objeto JSON
 * que contiene las métricas habilitadas en la configuración. Cada métrica activa
 * se mide mediante funciones auxiliares (no incluidas aquí) que obtienen los valores
 * actuales para CPU, memoria, disco, red, número de procesos e interrupciones de contexto.
 *
 * @param config Estructura `MetricsConfig` con las métricas que deben incluirse en el JSON.
 * @return char* Cadena JSON que contiene las métricas solicitadas. La cadena devuelta
 *         debe liberarse con `free` cuando ya no sea necesaria.
 */
char* create_metrics_json(MetricsConfig config);

/**
 * @brief Envía las métricas al monitor.
 *
 * Esta función lee la configuración de métricas desde un archivo JSON, genera un JSON
 * con las métricas seleccionadas y envía los datos a través de un pipe (FIFO) al monitor.
 * Si el JSON no se puede generar o el pipe no está disponible, se muestra un mensaje de error.
 */
void send_metrics_to_monitor();
