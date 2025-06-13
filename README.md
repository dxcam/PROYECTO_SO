<div style="text-align: center;">
  <img src="img\logoEPN.png" alt="Descripción de la imagen" />
</div>
<div align="center">

**ESCUELA POLITÉCNICA NACIONAL**

**FIS**

**SISTEMAS OPERATIVOS**

***PROYECTO BIMESTRE I***

**INTEGRANTES:**

  -Camila Caicedo

  -Joel Velastegui

  -Kenneth Yar
  
***GRUPO D***

**Fecha de entrega:** 11 de junio del 2025


</div>

___

# Tema: Ejecución de múltiples tareas con procesos y con hilos

## SIMULACIÓN DE UNA ADMINISTRACIÓN DE HOTEL

## Objetivo:

Explorar las diferencias de rendimiento entre dos modelos de ejecución concurrente (procesos y hilos) en lenguaje C, aplicados al contexto de un sistema de administración hotelera. El proyecto busca demostrar, mediante evidencia empírica, las ventajas de los hilos frente a los procesos tradicionales para tareas concurrentes.

## Problema:

Desarrollar dos versiones de un programa en C que procese un conjunto de transacciones simuladas divididas en 8 grupos (GRUPO A a GRUPO H). Cada grupo contiene un subconjunto de tareas relacionadas con la administración hotelera.

- **Versión 1: Procesos**
  
  * Se utilizó fork() para crear procesos hijos.

  * Cada proceso atiende un grupo de transacciones.

  * La sincronización entre procesos se realizó mediante uso de archivos temporales compartidos y espera activa.

- **Versión 2: Hilos**

  * Se utilizó la biblioteca pthread.h.

  * Cada hilo atiende un grupo de transacciones.

  * Se implementaron mutex para evitar condiciones de carrera al acceder a recursos compartidos.

En este caso, cada grupo representa un piso del hotel. Las actividades a ser simuladas son las reservas, disponibilidad y limpieza de habitaciones, check-ins y check-outs realizados. Además , la sincronización se puede lograr de manera efectiva al compartir variables de estado.


___
# PROCEDIMIENTO PARA LA EJECUCIÓN 

- **Versión 2: Hilos**

    Para ejecutar el programa con la simulación de la Administración de un hotel implementado con Hilos se usaron los siguientes comandos en la terminal:

    ```
    gcc -pthread HotelHilos.c -o HotelHilos

    ./HotelHilos
    ```

- **Versión 2: Procesos**

    Para ejecutar el programa con la simulación de la Administración de un hotel implementado mediante Procesos se usaron los siguientes comandos en la terminal:

    ```
    gcc -o HotelProcesos.c HotelProcesos

    ./HotelProcesos
    ```

> Adicionalmente, se creó otro programa en C para comparar el tiempo de ejecución de dos programas diferentes: uno que usa procesos y otro que usa hilos. Esto con el fin de medir y mostrar cuánto tiempo tarda cada uno en ejecutarse y determinar cuál es más rápido.

Para ejecutar este programa se usaron los comandos:

```
    gcc -o ComparaionTiempos.c ComparacionTiempos

    ./ComparacionTiempos
```


# Resultados

- **Implementación con Procesos:**
  
<div style="text-align: center;">
  <img src="img\p1.jpg" alt="Descripción de la imagen" />
  <img src="img\p2.jpg" alt="Descripción de la imagen" />
  <img src="img\p3.jpg" alt="Descripción de la imagen" />
  <img src="img\p4.jpg" alt="Descripción de la imagen" />
  <img src="img\p5.jpg" alt="Descripción de la imagen" />
</div>
<div align="center">

___

- **Implementación con Procesos:**

  <img src="img\h1.jpg" alt="Descripción de la imagen" />
  <img src="img\h2.jpg" alt="Descripción de la imagen" />
  <img src="img\h3.jpg" alt="Descripción de la imagen" />
  <img src="img\h4.jpg" alt="Descripción de la imagen" />
  <img src="img\h5.jpg" alt="Descripción de la imagen" />
</div>
<div align="center">

- **Comparación de Tiempos entre Hilos y Procesos**

<div style="text-align: center;">
  <img src="img\c.jpg" alt="Descripción de la imagen" />
</div>
<div align="center">


___
# Conclusiones

- Con la implementación hilos se observaron tiempos de ejecución más rápidos en comparación con la versión de procesos. Esto se debe a que los hilos comparten el mismo espacio de memoria, lo que reduce la sobrecarga de creación y gestión de recursos. Mientras que los procesos implican una mayor sobrecarga en su creación, ya que cada proceso tiene su propio espacio de memoria. Debido a esto, resulta en tiempos de ejecución más lentos.
- Una diferencia encontrada es la sincronización. En la versión de hilos se manejó mediante mutexes, que son más sencillos de implementar. En cambio, en la versión de procesos, la sincronización puede ser más compleja y requiere el uso de mecanismos como pipes.
- Los hilos se usan especialmete en aplicaciones donde la eficiencia y la velocidad tienen relevancia o se requiere compartir datos en tiempo real. Por ejemplo, en los juegos donde la comunicación entre estos hilos sincroniza la información y evita retrasos o errores visuales.
- Los procesos tienen un mejor uso en aplicaciones que requieren un alto grado de aislamiento entre tareas y no necesitan compartir información. Por ejemplo, en sistemas operativos, ya que cada programa que se abra en la computadora corre en su propio proceso.
- Durante la implementación del sistema de administración del hotel, se observó un comportamiento inesperado: los procesos se ejecutaban más rápidamente que los hilos. Para abordar este problema, se tomó la decisión de ejecutar el programa de manera más controlada, teniendo en cuenta la latencia inherente al procesamiento de hilos.