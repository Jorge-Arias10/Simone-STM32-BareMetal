/**
 * @file stm32f4_simone.c
 * @brief Portable functions to interact with the Simone FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Standard C includes */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h> // Used to compute the timer ARR and PSC values

/* HW dependent includes */
#include "stm32f4_simone.h"

/* Microcontroller dependent includes */
#include "port_simone.h"

/* Global variables */
stm32f4_simone_hw_t simone_hw;

/* Private functions ----------------------------------------------------------*/
static void _timer_simone_setup()
{
    /* TODO students: complete timer configuration */
    
    /* 1. Habilitar el reloj del TIM3 en el bus APB1 */
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;


    /* 2. Dejar el estado controlado (Punto 2 del correo de JOSUÉ) */
    TIM3->CR1 = 0;           // Limpiar configuración previa
    TIM3->CR1 |= TIM_CR1_ARPE; // Activar Auto-reload preload
    TIM3->SR &= ~TIM_SR_UIF;  // Limpiar cualquier flag de interrupción pendiente

    /* 3. Configurar la prioridad de la interrupción a 3 y subprioridad 0 (Petición del enunciado) */
    NVIC_SetPriority(TIM3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 3, 0));

    /* 4. Habilitar la interrupción del TIM3 en el controlador NVIC */
    NVIC_EnableIRQ(TIM3_IRQn);
}

void port_simone_set_timer_timeout(uint32_t duration_ms)
{

    /* PUNTO 1 del correo de JOSUÉ: Limpiar el flag global ANTES de empezar */
    port_simone_set_timeout_status(false);

    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->CNT = 0;

    uint32_t timer_clk_hz = SystemCoreClock;
    uint32_t tick_hz = 10000U; 
    uint32_t psc = (uint32_t)round(((double)timer_clk_hz / (double)tick_hz) - 1.0);
    uint32_t arr = (uint32_t)round(((double)duration_ms * (double)tick_hz / 1000.0) - 1.0);

    TIM3->PSC = psc;
    TIM3->ARR = arr;

    /* 1. Desactivamos las interrupciones para que no nos escuche */
    TIM3->DIER &= ~TIM_DIER_UIE;

    /* 2. Forzamos la recarga (esto levanta la bandera falsa por dentro) */
    TIM3->EGR = TIM_EGR_UG;

    /* 3. ¡LIMPIEZA PROFUNDA! Borramos la bandera falsa del Timer y del procesador */
    TIM3->SR &= ~TIM_SR_UIF;
    NVIC_ClearPendingIRQ(TIM3_IRQn);

    /* 4. Volvemos a activar las interrupciones de forma segura y arrancamos */
    TIM3->DIER |= TIM_DIER_UIE;
    TIM3->CR1 |= TIM_CR1_CEN;
}

void port_simone_stop_timer(void)
{
    TIM3->CR1 &= ~TIM_CR1_CEN;
    TIM3->DIER &= ~TIM_DIER_UIE;
}

bool port_simone_get_timeout_status(void)
{
    return simone_hw.flag_timer_timeout;
}

void port_simone_set_timeout_status(bool status)
{
    simone_hw.flag_timer_timeout = status;
}

void port_simone_init(void)
{
    /* TODO students: */
    // 1. Reset the flag of the timer timeout
    simone_hw.flag_timer_timeout = false;
    // 2. Configure the timer that controls the duration of the different aspects of the game.
    _timer_simone_setup();
}