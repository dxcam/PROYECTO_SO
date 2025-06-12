#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>

// Colores para mejorar visualización en consola
#define ROJO     "\x1B[31m"
#define VERDE    "\x1B[32m"
#define AMARILLO "\x1B[33m"
#define AZUL     "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CYAN     "\x1B[36m"
#define BLANCO   "\x1B[37m"
#define RESET    "\x1B[0m"

// Constantes
#define NUM_GRUPOS 8
#define OPERACIONES_POR_GRUPO 5

// Estructura para datos del hotel
typedef struct {
    int habitaciones_disponibles;
    int reservaciones;
    int ingresos;
    int salidas;
    pthread_mutex_t mutex;
} DatosHotel;

// Argumentos que se pasan a cada hilo
typedef struct {
    char grupo;
    DatosHotel *datos;
    struct timespec *inicio;
    struct timespec *fin;
} ArgumentosHilo;

// Manejador de errores
void manejar_error(const char *msg, int errnum) {
    fprintf(stderr, ROJO "Error: %s: %s\n" RESET, msg, strerror(errnum));
    exit(EXIT_FAILURE);
}

// Obtener hora actual como cadena
void obtener_hora_actual(char *buffer, size_t size) {
    time_t tiempo_crudo;
    struct tm *info_tiempo;
    time(&tiempo_crudo);
    info_tiempo = localtime(&tiempo_crudo);
    strftime(buffer, size, "%H:%M:%S", info_tiempo);
}

// Muestra el estado actual del hotel
void mostrar_estado_hotel(DatosHotel *datos) {
    printf("\n" AZUL "╔════════════════════════════════════════╗\n");
    printf("║" BLANCO "        ESTADO ACTUAL DEL HOTEL       " AZUL "  ║\n");
    printf("╠════════════════════════════════════════╣\n");
    printf("║" BLANCO " Habitaciones disponibles: " VERDE "%2d        " AZUL "   ║\n", datos->habitaciones_disponibles);
    printf("║" BLANCO " Reservaciones activas:    " AMARILLO "%2d        " AZUL "   ║\n", datos->reservaciones);
    printf("║" BLANCO " Check-ins realizados:     " CYAN "%2d        " AZUL "   ║\n", datos->ingresos);
    printf("║" BLANCO " Check-outs realizados:    " MAGENTA "%2d        " AZUL "   ║\n", datos->salidas);
    printf("╚════════════════════════════════════════╝\n" RESET);
}

// Muestra el tiempo de ejecución de cada grupo
void mostrar_tiempo_grupo(char grupo, const char *inicio, const char *fin, double duracion) {
    printf(AMARILLO "\n┌──────────────────────────────────────────┐\n");
    printf("│           TIEMPOS DEL GRUPO %c            │\n", grupo);
    printf("├──────────────────────────────────────────┤\n");
    printf("│ Inicio:  %s                        │\n", inicio);
    printf("│ Fin:     %s                        │\n", fin);
    printf("│ Duración: %.6f segundos              │\n", duracion);
    printf("└──────────────────────────────────────────┘\n" RESET);
}

// Devuelve el tipo de operación según número
const char* obtener_tipo_operacion(int num_operacion) {
    switch(num_operacion % 4) {
        case 0: return "Reservación de habitación";
        case 1: return "Check-in de huésped";
        case 2: return "Check-out de huésped";
        case 3: return "Limpieza de habitación";
        default: return "Operación desconocida";
    }
}

// Ejecuta la operación del hotel
void procesar_operacion(char grupo, int num_operacion, DatosHotel *datos) {
    const char *accion = obtener_tipo_operacion(num_operacion);
    printf("\nGrupo %c - %s\n", grupo, accion);

    // Sección crítica protegida con mutex
    int bloqueo = pthread_mutex_lock(&datos->mutex);
    if (bloqueo != 0) {
        manejar_error("pthread_mutex_lock", bloqueo);
    }

    switch(num_operacion % 4) {
        case 0: // Reservación
            if (datos->habitaciones_disponibles > 0) {
                datos->habitaciones_disponibles--;
                datos->reservaciones++;
                printf("  Reservación exitosa. Habitaciones disponibles: %d\n", datos->habitaciones_disponibles);
            } else {
                printf("  No hay habitaciones disponibles\n");
            }
            break;
        case 1: // Check-in
            if (datos->reservaciones > 0) {
                datos->reservaciones--;
                datos->ingresos++;
                printf("  Check-in realizado. Huéspedes actuales: %d\n", datos->ingresos);
            } else {
                printf("  No hay reservaciones para check-in\n");
            }
            break;
        case 2: // Check-out
            if (datos->ingresos > 0) {
                datos->ingresos--;
                datos->habitaciones_disponibles++;
                datos->salidas++;
                printf("  Check-out realizado. Habitaciones liberadas: %d\n", datos->habitaciones_disponibles);
            } else {
                printf("  No hay huéspedes para check-out\n");
            }
            break;
        case 3: // Limpieza
            printf("  Habitación limpiada y marcada como disponible\n");
            break;
    }

    // Fin sección crítica
    int desbloqueo = pthread_mutex_unlock(&datos->mutex);
    if (desbloqueo != 0) {
        manejar_error("pthread_mutex_unlock", desbloqueo);
    }

    // Simula tiempo real de operación
    usleep(200000 * (rand() % 3 + 1));
}

// Función que ejecuta cada hilo
void *funcion_hilo(void *arg) {
    ArgumentosHilo *argumentos = (ArgumentosHilo *)arg;
    char grupo = argumentos->grupo;
    DatosHotel *datos = argumentos->datos;

    char hora_inicio[20], hora_fin[20];
    struct timespec inicio, fin;

    if (clock_gettime(CLOCK_MONOTONIC, &inicio) == -1) {
        manejar_error("clock_gettime (inicio)", errno);
    }
    obtener_hora_actual(hora_inicio, sizeof(hora_inicio));

    printf(CYAN "\n■■■■■ Grupo %c iniciado ■■■■■\n" RESET, grupo);

    for (int i = 1; i <= OPERACIONES_POR_GRUPO; i++) {
        procesar_operacion(grupo, i, datos);
    }

    if (clock_gettime(CLOCK_MONOTONIC, &fin) == -1) {
        manejar_error("clock_gettime (fin)", errno);
    }
    obtener_hora_actual(hora_fin, sizeof(hora_fin));

    double duracion = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;
    mostrar_tiempo_grupo(grupo, hora_inicio, hora_fin, duracion);

    pthread_exit(NULL);
}

int main() {
    struct timespec inicio_total, fin_total;
    pthread_t hilos[NUM_GRUPOS];
    ArgumentosHilo argumentos[NUM_GRUPOS];
    DatosHotel datos_compartidos = {10, 0, 0, 0};

    // Inicializar mutex
    if (pthread_mutex_init(&datos_compartidos.mutex, NULL) != 0) {
        manejar_error("pthread_mutex_init", errno);
    }

    // Cabecera principal
    printf(ROJO "\n╔════════════════════════════════════════════╗\n");
    printf("║" BLANCO "   SISTEMA DE ADMINISTRACIÓN DE HOTEL   " ROJO "    ║\n");
    printf("║" BLANCO "      (Versión Multihilo con pthread)   " ROJO "    ║\n");
    printf("╚════════════════════════════════════════════╝\n\n" RESET);

    mostrar_estado_hotel(&datos_compartidos);
    printf("\nPresione Enter para comenzar...");
    getchar();

    char hora_inicio_total[20], hora_fin_total[20];
    obtener_hora_actual(hora_inicio_total, sizeof(hora_inicio_total));

    if (clock_gettime(CLOCK_MONOTONIC, &inicio_total) == -1) {
        manejar_error("clock_gettime (main inicio)", errno);
    }

    // Crear hilos
    for (int i = 0; i < NUM_GRUPOS; i++) {
        argumentos[i].grupo = 'A' + i;
        argumentos[i].datos = &datos_compartidos;

        if (pthread_create(&hilos[i], NULL, funcion_hilo, &argumentos[i]) != 0) {
            manejar_error("pthread_create", errno);
        }
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_GRUPOS; i++) {
        if (pthread_join(hilos[i], NULL) != 0) {
            manejar_error("pthread_join", errno);
        }
    }

    if (clock_gettime(CLOCK_MONOTONIC, &fin_total) == -1) {
        manejar_error("clock_gettime (main fin)", errno);
    }
    obtener_hora_actual(hora_fin_total, sizeof(hora_fin_total));

    double duracion_total = (fin_total.tv_sec - inicio_total.tv_sec) + 
                            (fin_total.tv_nsec - inicio_total.tv_nsec) / 1e9;

    printf(ROJO "\n╔════════════════════════════════════════════╗\n");
    printf("║" BLANCO "           RESULTADOS FINALES           " ROJO "    ║\n");
    printf("╚════════════════════════════════════════════╝\n" RESET);

    mostrar_estado_hotel(&datos_compartidos);
    printf("\nInicio total: %s\n", hora_inicio_total);
    printf("Fin total:    %s\n", hora_fin_total);
    printf("Duración total: %.6f segundos\n", duracion_total);


    // Liberar recursos
    if (pthread_mutex_destroy(&datos_compartidos.mutex) != 0) {
        manejar_error("pthread_mutex_destroy", errno);
    }

    return EXIT_SUCCESS;
}

