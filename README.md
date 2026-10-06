# Proyecto Simone - Juego de Memoria con FSM

Acceso a la documentación y API del proyecto en [este enlace](https://sdg2dieupm.github.io/simone/).

## Authors

* **Jorge Arias Ávila** - email: [jo.arias@alumno.upm.es](mailto:alumno@alumno.upm.es)

**Descripción del Proyecto (ES):** 
Este proyecto implementa el clásico juego de memoria "Simone" (estilo Simon Says) sobre una placa STM32F4. El sistema genera secuencias aleatorias de colores a través de un LED RGB que el jugador debe repetir usando un teclado matricial. La arquitectura de software está diseñada de forma modular utilizando Máquinas de Estados Finitos (FSM) para cada periférico, temporizadores hardware precisos y modos de bajo consumo (Sleep Mode) para optimizar la batería durante los tiempos de inactividad.

**Project Description (EN):**
This project implements the classic memory game "Simone" (similar to Simon Says) on an STM32F4 board. The system generates random color sequences using an RGB LED, which the player must repeat using a matrix keypad. The software architecture is highly modular, using Finite State Machines (FSM) for each peripheral, precise hardware timers, and low-power modes (Sleep Mode) to optimize battery life during idle times.

Puede añadir una imagen de portada **de su propiedad** aquí. Por ejemplo, del montaje final, o una captura de osciloscopio, etc.

[![Demostración Proyecto Simone](docs/assets/imgs/otra_FOTO)](https://youtu.be/tG3hYmJkZ0k "Demostración final del proyecto Simone en la placa STM32")


## Version 1: FSM Button

Implementación de la máquina de estados para la lectura segura del botón de usuario. 
* Gestión de rebotes mecánicos (*debounce*) mediante comprobación de *timeouts*.
* Registro del tiempo de pulsación para discriminar entre pulsaciones cortas y largas (necesarias para encender o apagar el juego Simone).

## Version 2: FSM Keyboard

Desarrollo de la máquina de estados para el escaneo continuo del teclado matricial. 
* Excitación secuencial de las filas del teclado.
* Lectura de las columnas mediante interrupciones hardware (EXTI).
* Gestión de *debounce* y traducción de la posición física a un carácter lógico válido.

## Version 3: FSM RGB Light

Implementación de la máquina de estados encargada del *feedback* visual del sistema. 
* Control del LED RGB para mostrar la secuencia del juego y los aciertos/fallos.
* Soporte para múltiples colores mediante la combinación de pines digitales.
* Gestión de intensidades para adaptar la dificultad visual a los diferentes niveles del juego.

## Version 4: FSM Simone (Core)

Diseño e implementación de la máquina de estados principal que orquesta la lógica del juego.
* **Generación de Secuencia:** Creación aleatoria de colores según la longitud de la ronda actual (`ADD_COLOR`).
* **Reproducción:** Mostrar al usuario la secuencia con tiempos de encendido y apagado controlados por un Timer Hardware de 16 bits (`PLAYBACK`).
* **Interacción:** Gestión del tiempo de espera del jugador (`WAIT_KEY`) y verificación estricta de las teclas pulsadas (`VERIFY_INPUT`).
* **Progresión:** Aumento dinámico del nivel de dificultad tras completar rondas con éxito.

## Diagrama de Estados del Sistema:

![Diagrama de Estados de Sistema Simone](docs/assets/imgs/DIAGRAMA_FSM_SISTEMA.jpg)
