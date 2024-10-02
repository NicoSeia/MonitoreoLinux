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
 * 
 * @param disk_name Nombre del disco (por ejemplo, "sda1") del cual se obtiene la métrica.
 */
void update_disk_gauge(const char* disk_name);

/**
 * @brief Actualiza la métrica de uso de la red.
 * 
 * @param interface_name Nombre de la red (por ejemplo, "enp1s0") del cual se obtiene la métrica.
 */
void update_net_gauge(const char* interface_name);

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
