#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main() {
    printf("=== COMPARACIÓN DE TIEMPOS ===\n");

    // Medir el tiempo de ejecución del programa con procesos
    struct timespec inicio_procesos, fin_procesos;
    clock_gettime(CLOCK_MONOTONIC, &inicio_procesos);
    system("./hotel_procesos"); // Ejecuta el programa que usa procesos
    clock_gettime(CLOCK_MONOTONIC, &fin_procesos);

    // Calcula la duración total
    double tiempo_procesos = (fin_procesos.tv_sec - inicio_procesos.tv_sec) +
                             (fin_procesos.tv_nsec - inicio_procesos.tv_nsec) / 1e9;

    // Medir el tiempo de ejecución del programa con hilos
    struct timespec inicio_hilos, fin_hilos;
    clock_gettime(CLOCK_MONOTONIC, &inicio_hilos);
    system("./hotel_hilos"); // Ejecuta el programa que usa hilos
    clock_gettime(CLOCK_MONOTONIC, &fin_hilos);

    // Calcula la duración total
    double tiempo_hilos = (fin_hilos.tv_sec - inicio_hilos.tv_sec) +
                          (fin_hilos.tv_nsec - inicio_hilos.tv_nsec) / 1e9;

    // Mostrar los resultados
    printf("\n=== Resultados Comparativos ===\n");
    printf("Tiempo con procesos: %.6f segundos\n", tiempo_procesos);
    printf("Tiempo con hilos:    %.6f segundos\n", tiempo_hilos);
    printf("Diferencia:          %.6f segundos (%.2f%% más rápido)\n",
           tiempo_procesos - tiempo_hilos,
           ((tiempo_procesos - tiempo_hilos) / tiempo_procesos) * 100);

    return 0;
}

