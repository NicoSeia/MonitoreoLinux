/**
 * @file expose_metrics.h
 * @brief Programa para leer el uso de CPU y memoria y exponerlos como métricas de Prometheus.
 */

#include "metrics.h"
#include <errno.h>
#include <prom.h>
#include <promhttp.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // Para sleep

/**
 * @brief Tamaño del buffer utilizado para leer datos del sistema de archivos /proc.
 *
 * Esta constante define el tamaño del buffer que se utiliza para almacenar
 * temporalmente los datos leídos desde archivos del sistema.
 */
#define BUFFER_SIZE 256

/**
 * @brief Actualiza la métrica de uso de CPU.
 */
void update_cpu_gauge();

/**
 * @brief Actualiza la métrica de uso de memoria.
 */
void update_memory_gauge();

/**
 * @brief Actualiza la métrica de uso del estado del disco.
 */
void update_disk_gauge();

/**
 * @brief Actualiza la métrica de uso de la red.
 */
void update_network_gauge();

/**
 * @brief Actualiza la métrica de procesos en ejecucion.
 */
void update_procs_gauge();

/**
 * @brief Actualiza la métrica de cambios de contexto desde que inicio el sistema.
 */
void update_ctxt_gauge();

/**
 * @brief Función del hilo para exponer las métricas vía HTTP en el puerto 8000.
 * @param arg Argumento no utilizado.
 * @return NULL
 */
void* expose_metrics(void* arg);

/**
 * @brief Inicializar mutex y métricas.
 */
void init_metrics();

/**
 * @brief Destructor de mutex
 */
void destroy_mutex();
