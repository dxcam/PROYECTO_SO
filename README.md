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

___
# PROCEDIMIENTO PARA LA EJECUCIÓN 

- **Versión 2: Hilos**

    Para ejecutar el programa con la simulación de la Administración de un hotel implementado con Hilos se usaron los siguientes comandos en la terminal:

    ```
    gcc -pthread hotel_hilo.c -o hotel_hilo

    ./hotel_hilo
    ```

- **Versión 2: Procesos**

    Para ejecutar el programa con la simulación de la Administración de un hotel implementado mediante Procesos se usaron los siguientes comandos en la terminal:

    ```
    gcc -o hotel_proceso.c hotel_proceso

    ./hotel_proceso
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