/**
 * @file main.c
 * @brief Entry point of the system
 */

#include "../include/expose_metrics.h"
#include "../include/metrics.h"
#include "expose_metrics.c"
#include "metrics.c"
#include <stdbool.h>

/**
 * @brief Tiempo de espera en segundos entre actualizaciones de métricas.
 */
#define SLEEP_TIME 1

/**
 * @brief Entry point of the system.
 *
 * Este es el punto de entrada de la aplicación. Se encarga de inicializar las métricas,
 * crear un hilo para exponer las métricas a través de HTTP y actualizar periódicamente
 * las métricas del sistema en un bucle infinito.
 *
 * @param argc Número de argumentos de línea de comandos.
 * @param argv Array de argumentos de línea de comandos.
 * @return EXIT_SUCCESS si la ejecución fue exitosa, EXIT_FAILURE en caso de error.
 */
int main(int argc, char* argv[])
{
    init_metrics();
    // Creamos un hilo para exponer las métricas vía HTTP
    pthread_t tid;
    if (pthread_create(&tid, NULL, expose_metrics, NULL) != 0)
    {
        fprintf(stderr, "Error al crear el hilo del servidor HTTP\n");
        return EXIT_FAILURE;
    }

    // Bucle principal para actualizar las métricas cada segundo
    while (true)
    {
        update_cpu_gauge();
        update_memory_gauge();
        update_disk_gauge("sda");
        update_network_gauge();
        update_procs_gauge();
        update_ctxt_gauge();
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}
