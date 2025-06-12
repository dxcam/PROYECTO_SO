#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#define ROJO     "\x1B[31m"
#define VERDE    "\x1B[32m"
#define AMARILLO "\x1B[33m"
#define AZUL     "\x1B[34m"
#define MAGENTA  "\x1B[35m"
#define CIAN     "\x1B[36m"
#define BLANCO   "\x1B[37m"
#define RESET    "\x1B[0m"

#define NUM_GRUPOS 8
#define TRANSACCIONES_POR_GRUPO 5

// Estructura que representa el estado del hotel
typedef struct {
    int habitaciones_disponibles;
    int reservaciones;
    int check_ins;
    int check_outs;
} DatosHotel;

// Estructura para inicializar semáforos
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

// Función para manejar errores del sistema
void manejar_error(const char *mensaje) {
    perror(mensaje);
    exit(EXIT_FAILURE);
}

// Obtener la hora actual en formato HH:MM:SS
void obtener_hora_actual(char *buffer, size_t tamaño) {
    time_t tiempo_bruto;
    struct tm *info_tiempo;
    time(&tiempo_bruto);
    info_tiempo = localtime(&tiempo_bruto);
    strftime(buffer, tamaño, "%H:%M:%S", info_tiempo);
}

// Imprimir el estado del hotel en una caja visual bien alineada
void mostrar_estado_hotel(DatosHotel *datos) {
    printf("\n" AZUL "╔════════════════════════════════════════════╗\n");
    printf("║" BLANCO "         ESTADO ACTUAL DEL HOTEL            " AZUL "║\n");
    printf("╠════════════════════════════════════════════╣\n");
    printf("║" BLANCO " Habitaciones disponibles: " VERDE "%2d               " AZUL "║\n", datos->habitaciones_disponibles);
    printf("║" BLANCO " Reservaciones activas:    " AMARILLO "%2d               " AZUL "║\n", datos->reservaciones);
    printf("║" BLANCO " Check-ins realizados:     " CIAN "%2d               " AZUL "║\n", datos->check_ins);
    printf("║" BLANCO " Check-outs realizados:    " MAGENTA "%2d               " AZUL "║\n", datos->check_outs);
    printf("╚════════════════════════════════════════════╝\n" RESET);
}

// Imprimir los tiempos de ejecución de cada grupo
void mostrar_tiempos_grupo(char grupo, const char *inicio, const char *fin, double duracion) {
    printf(AMARILLO "\n┌────────────────────────────────────────────┐\n");
    printf("│           TIEMPOS DEL GRUPO %c              │\n", grupo);
    printf("├────────────────────────────────────────────┤\n");
    printf("│ Inicio:   %-32s │\n", inicio);
    printf("│ Fin:      %-32s │\n", fin);
    printf("│ Duración: %-9.6f segundos               │\n", duracion);
    printf("└────────────────────────────────────────────┘\n" RESET);
}

// Retornar el tipo de transacción basado en el número
const char* obtener_tipo_transaccion(int num_transaccion) {
    switch(num_transaccion % 4) {
        case 0: return "Reservación de habitación";
        case 1: return "Check-in de huésped";
        case 2: return "Check-out de huésped";
        case 3: return "Limpieza de habitación";
        default: return "Operación desconocida";
    }
}

// Operación de espera del semáforo (bloqueo)
void esperar_semaforo(int semid) {
    struct sembuf sb = {0, -1, 0};
    if (semop(semid, &sb, 1) == -1) {
        manejar_error("semop wait");
    }
}

// Operación de señal del semáforo (desbloqueo)
void señalar_semaforo(int semid) {
    struct sembuf sb = {0, 1, 0};
    if (semop(semid, &sb, 1) == -1) {
        manejar_error("semop signal");
    }
}

// Procesar una transacción específica de hotel
void procesar_transaccion(char grupo, int num_transaccion, DatosHotel *datos, int semid) {
    const char *accion = obtener_tipo_transaccion(num_transaccion);
    printf("\nGrupo %c - %s\n", grupo, accion);
    
    esperar_semaforo(semid);

    // Actualizar el estado del hotel según la transacción
    switch(num_transaccion % 4) {
        case 0:
            if(datos->habitaciones_disponibles > 0) {
                datos->habitaciones_disponibles--;
                datos->reservaciones++;
                printf("  Reservación exitosa. Habitaciones disponibles: %d\n", datos->habitaciones_disponibles);
            } else {
                printf("  No hay habitaciones disponibles\n");
            }
            break;
        case 1:
            if(datos->reservaciones > 0) {
                datos->reservaciones--;
                datos->check_ins++;
                printf("  Check-in realizado. Huéspedes actuales: %d\n", datos->check_ins);
            } else {
                printf("  No hay reservaciones para check-in\n");
            }
            break;
        case 2:
            if(datos->check_ins > 0) {
                datos->check_ins--;
                datos->habitaciones_disponibles++;
                datos->check_outs++;
                printf("  Check-out realizado. Habitaciones liberadas: %d\n", datos->habitaciones_disponibles);
            } else {
                printf("  No hay huéspedes para check-out\n");
            }
            break;
        case 3:
            printf("  Habitación limpiada y marcada como disponible\n");
            break;
    }

    señalar_semaforo(semid);
    usleep(200000 * (rand() % 3 + 1)); // Simulación de tiempo de operación
}

int main() {
    struct timespec inicio, fin;
    int id_memoria, id_semaforo;
    DatosHotel *datos_compartidos;
    key_t clave = ftok(".", 'H');

    if (clave == -1) {
        manejar_error("ftok");
    }

    id_memoria = shmget(clave, sizeof(DatosHotel), IPC_CREAT | 0666);
    if (id_memoria == -1) {
        manejar_error("shmget");
    }

    datos_compartidos = (DatosHotel *)shmat(id_memoria, NULL, 0);
    if (datos_compartidos == (void *)-1) {
        manejar_error("shmat");
    }

    // Inicializar los valores del hotel
    datos_compartidos->habitaciones_disponibles = 10;
    datos_compartidos->reservaciones = 0;
    datos_compartidos->check_ins = 0;
    datos_compartidos->check_outs = 0;

    // Crear e inicializar el semáforo
    id_semaforo = semget(clave, 1, IPC_CREAT | 0666);
    if (id_semaforo == -1) {
        manejar_error("semget");
    }

    union semun su;
    su.val = 1;
    if (semctl(id_semaforo, 0, SETVAL, su) == -1) {
        manejar_error("semctl");
    }

    printf(ROJO "\n╔════════════════════════════════════════════╗\n");
    printf("║" BLANCO "   SISTEMA DE ADMINISTRACIÓN DE HOTEL   " ROJO "    ║\n");
    printf("║" BLANCO "    (Versión Multiproceso con fork)     " ROJO "    ║\n");
    printf("╚════════════════════════════════════════════╝\n\n" RESET);
    
    mostrar_estado_hotel(datos_compartidos);
    printf("\nPresione Enter para comenzar...");
    getchar();

    char hora_inicio[20], hora_fin[20];
    obtener_hora_actual(hora_inicio, sizeof(hora_inicio));
    if (clock_gettime(CLOCK_MONOTONIC, &inicio) == -1) {
        manejar_error("clock_gettime (inicio)");
    }

    // Crear procesos hijos para simular los grupos
    for(char grupo = 'A'; grupo < 'A' + NUM_GRUPOS; grupo++) {
        pid_t pid = fork();
        if(pid == -1) {
            manejar_error("fork");
        } else if(pid == 0) {
            char inicio_grupo[20], fin_grupo[20];
            struct timespec tiempo_ini, tiempo_fin;

            srand(getpid()); // Semilla única para cada hijo

            obtener_hora_actual(inicio_grupo, sizeof(inicio_grupo));
            if (clock_gettime(CLOCK_MONOTONIC, &tiempo_ini) == -1) {
                manejar_error("clock_gettime (hijo inicio)");
            }

            printf(CIAN "\n■■■■■ Grupo %c iniciado (PID: %d) ■■■■■\n" RESET, grupo, getpid());

            for(int i = 1; i <= TRANSACCIONES_POR_GRUPO; i++) {
                procesar_transaccion(grupo, i, datos_compartidos, id_semaforo);
            }

            obtener_hora_actual(fin_grupo, sizeof(fin_grupo));
            if (clock_gettime(CLOCK_MONOTONIC, &tiempo_fin) == -1) {
                manejar_error("clock_gettime (hijo fin)");
            }

            double duracion = (tiempo_fin.tv_sec - tiempo_ini.tv_sec) +
                              (tiempo_fin.tv_nsec - tiempo_ini.tv_nsec) / 1e9;
            mostrar_tiempos_grupo(grupo, inicio_grupo, fin_grupo, duracion);
            exit(EXIT_SUCCESS);
        }
    }

    // Esperar a que terminen los procesos hijos
    for(int i = 0; i < NUM_GRUPOS; i++) {
        int estado;
        pid_t pid_hijo = wait(&estado);
        if(pid_hijo == -1) {
            manejar_error("wait");
        }

        if(WIFEXITED(estado) && WEXITSTATUS(estado) != 0) {
            fprintf(stderr, ROJO "Proceso hijo %d terminó con código %d\n" RESET, pid_hijo, WEXITSTATUS(estado));
        } else if(!WIFEXITED(estado)) {
            fprintf(stderr, ROJO "Proceso hijo %d terminó anormalmente\n" RESET, pid_hijo);
        }
    }

    obtener_hora_actual(hora_fin, sizeof(hora_fin));
    if (clock_gettime(CLOCK_MONOTONIC, &fin) == -1) {
        manejar_error("clock_gettime (fin)");
    }

    double tiempo_total = (fin.tv_sec - inicio.tv_sec) + (fin.tv_nsec - inicio.tv_nsec) / 1e9;

    printf(ROJO "\n╔════════════════════════════════════════════╗\n");
    printf("║" BLANCO "           RESULTADOS FINALES           " ROJO "    ║\n");
    printf("╚════════════════════════════════════════════╝\n" RESET);

    mostrar_estado_hotel(datos_compartidos);
    printf("\nInicio total: %s\n", hora_inicio);
    printf("Fin total:    %s\n", hora_fin);
    printf("Duración total: %.6f segundos\n", tiempo_total);

    // Liberar memoria compartida y semáforo
    if (shmdt(datos_compartidos) == -1) {
        manejar_error("shmdt");
    }
    if (shmctl(id_memoria, IPC_RMID, NULL) == -1) {
        manejar_error("shmctl");
    }
    if (semctl(id_semaforo, 0, IPC_RMID) == -1) {
        manejar_error("semctl remove");
    }

    return EXIT_SUCCESS;
}

