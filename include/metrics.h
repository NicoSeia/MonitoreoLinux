/**
 * @file metrics.h
 * @brief Funciones para obtener el uso de CPU y memoria desde el sistema de archivos /proc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief Tamaño del buffer utilizado para leer datos del sistema de archivos /proc.
 *
 * Esta constante define el tamaño del buffer que se utiliza para almacenar
 * temporalmente los datos leídos desde archivos del sistema.
 */
#define BUFFER_SIZE 256

/**
 * @brief Obtiene el porcentaje de uso de memoria desde /proc/meminfo.
 *
 * Lee los valores de memoria total y disponible desde /proc/meminfo y calcula
 * el porcentaje de uso de memoria.
 *
 * @return Uso de memoria como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_memory_usage();

/**
 * @brief Obtiene el porcentaje de uso de CPU desde /proc/stat.
 *
 * Lee los tiempos de CPU desde /proc/stat y calcula el porcentaje de uso de CPU
 * en un intervalo de tiempo.
 *
 * @return Uso de CPU como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_cpu_usage();

/**
 * @brief Obtiene el porcentaje de uso de disco para un dispositivo específico.
 *
 * Lee las estadísticas del disco desde /proc/diskstats y calcula el porcentaje de uso
 * basado en el tiempo de lectura y escritura.
 *
 * @param disk_name Nombre del disco para el cual se desea calcular el uso.
 * @return Uso del disco como porcentaje (0.0 a 100.0), o -1.0 en caso de error.
 */
double get_disk_usage(const char* disk_name);

/**
 * @brief Obtiene el uso de red de una interfaz específica.
 *
 * Lee las estadísticas de paquetes desde /proc/net/dev para la interfaz de red
 * especificada y almacena la información sobre los paquetes recibidos y transmitidos.
 *
 * @param interface_name Nombre de la interfaz de red para la cual se desea obtener
 *        las estadísticas.
 * @return Estructura NetUsage que contiene los datos de uso de la interfaz de red.
 */
NetUsage get_net_usage(const char* interface_name);

/**
 * @brief Obtiene el número de procesos en ejecución desde /proc/stat.
 *
 * Lee el número total de procesos en ejecución desde /proc/stat y lo devuelve.
 *
 * @return Número de procesos en ejecución, o -1 en caso de error.
 */
int get_process_usage();

/**
 * @brief Obtiene el numero de cambios de contexto.
 *
 * Lee los valores necesarios para calcular el contexto de uso del sistema.
 * 
 * @return Numero de cambios de contexto, o -1 en caso de error.
 */
double get_ctxt_usage();