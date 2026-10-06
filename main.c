/**
 * @file main.c
 * @brief Main file. Integration of HW-SW for Simone game.
 * @author Sistemas Digitales II
 * @date 2026-01-01
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C libraries */
#include <stdio.h>

/* HW libraries */
#include "port_system.h"

/* FSM Project Includes */
#include "fsm_button.h"
#include "fsm_keyboard.h"
#include "fsm_rgb_light.h"
#include "fsm_simone.h"
#include "port_rgb_light.h"
#include "port_button.h"    
#include "port_keyboard.h"

/* Defines ------------------------------------------------------------------*/
/** @brief 3 segundos en milisegundos para encender/apagar el juego */
#define SIMONE_ON_OFF_PRESS_TIME_MS 3000

/**
 * @brief  El main del sistema.
 */
int main(void)
{
    /* 1. Init board (Inicialización del hardware) */
    port_system_init();

    /* 2. Creación e inicialización de las FSM de los periféricos */
    fsm_button_t *p_fsm_button = fsm_button_new(PORT_USER_BUTTON_DEBOUNCE_TIME_MS, PORT_USER_BUTTON_ID);
    fsm_keyboard_t *p_fsm_keyboard = fsm_keyboard_new(PORT_KEYBOARD_MAIN_DEBOUNCE_TIME_MS, PORT_KEYBOARD_MAIN_ID);
    fsm_rgb_light_t *p_fsm_rgb_light = fsm_rgb_light_new(PORT_RGB_LIGHT_ID); 

    /* 3. Creación de la FSM principal del sistema Simone */
    fsm_simone_t *p_fsm_simone = fsm_simone_new(p_fsm_button, SIMONE_ON_OFF_PRESS_TIME_MS, p_fsm_keyboard, p_fsm_rgb_light, LEVEL_EASY);

    /* Infinite loop */
    while (1)
    {
        /* 4. Lanzar las máquinas de estado de los periféricos de entrada/salida primero */
        fsm_button_fire(p_fsm_button);
        fsm_keyboard_fire(p_fsm_keyboard);
        fsm_rgb_light_fire(p_fsm_rgb_light);

        /* 5. Lanzar la máquina de Simone en último lugar */
        fsm_simone_fire(p_fsm_simone);

    } // End of while(1)

    /* 6. Liberación de memoria dinámica (malloc) */
    fsm_simone_destroy(p_fsm_simone);
    fsm_rgb_light_destroy(p_fsm_rgb_light);
    fsm_keyboard_destroy(p_fsm_keyboard);
    fsm_button_destroy(p_fsm_button);

    return 0;
}