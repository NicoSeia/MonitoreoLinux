/**
 * @file main.c
 * @brief Entry point of the system
 */

#include "../include/expose_metrics.h"
#include "../include/metrics.h"
#include "expose_metrics.c"
#include <stdbool.h>

#define SLEEP_TIME 1

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
        update_net_gauge("enp1s0");
        update_procs_gauge();
        update_ctxt_gauge();
        sleep(SLEEP_TIME);
    }

    return EXIT_SUCCESS;
}
