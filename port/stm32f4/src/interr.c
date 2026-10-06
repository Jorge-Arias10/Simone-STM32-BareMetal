/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author SDG2. Román Cárdenas (r.cardenas@upm.es) and Josué Pagán (j.pagan@upm.es)
 * @date 2026-01-01
 */
// Include HW dependencies:
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_button.c"
#include "stm32f4xx.h"

// Include headers of different port elements:
#include "port_keyboard.h"
#include "stm32f4_keyboard.h"
#include "stm32f4_button.h"
#include "port_simone.h"

/**
 * @brief Check and handle the column interrupt for a given column index.
 *
 * @param column_index Index of the column to check.
 */
static void _check_column_interrupt(uint8_t column_index)
{
    GPIO_TypeDef *p_port = keyboards_arr[PORT_KEYBOARD_MAIN_ID].p_col_ports[column_index];
    uint8_t pin = keyboards_arr[PORT_KEYBOARD_MAIN_ID].p_col_pins[column_index];

    uint8_t gpio_value = stm32f4_system_gpio_read(p_port, pin);

    if (gpio_value == 1U)
    {
        port_keyboard_set_key_pressed_status(PORT_KEYBOARD_MAIN_ID, true);
    }
    else
    {
        port_keyboard_set_key_pressed_status(PORT_KEYBOARD_MAIN_ID, false);
        keyboards_arr[PORT_KEYBOARD_MAIN_ID].col_idx_interrupt = column_index;
    }

    EXTI->PR |= (1U << pin);
}

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 *
 * @note This ISR is called when the SysTick timer generates an interrupt.
 * The program flow jumps to this ISR and increments the tick counter by one millisecond.
 *
 * > **TO-DO alumnos:**
 * >
 * > ✅ 1. **Increment the System tick counter `msTicks` in 1 count.** To do so, use the function `port_system_get_millis()` and `port_system_set_millis()`.
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified by a call of an ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition). **Added to the definition** after *static*.
 *
 */
void SysTick_Handler(void)
{
    uint32_t ticks_actuales = port_system_get_millis();
    port_system_set_millis(ticks_actuales + 1);
}

/**
 * @brief This function identifies the line/ pin which has raised the interruption. Then, perform the desired action. Before leaving it cleans the interrupt pending register.
 *
 * @return * void
 */
void EXTI15_10_IRQHandler(void)
{
    port_system_systick_resume();
    /* ISR user button */
    if (EXTI->PR & BIT_POS_TO_MASK(buttons_arr[PORT_USER_BUTTON_ID].pin))
    {
        /* 1. Read GPIO value of the user button */
        uint8_t gpio_value = stm32f4_system_gpio_read(
            buttons_arr[PORT_USER_BUTTON_ID].p_port,
            buttons_arr[PORT_USER_BUTTON_ID].pin);

        /* 2 & 3. Update flag_pressed depending on GPIO level */
        // si es high (alto), es pulsado
        if (gpio_value == 1)
        {
            /* Button released */
            buttons_arr[PORT_USER_BUTTON_ID].flag_pressed = false;
        }
        else
        {
            /* Button pressed */
            buttons_arr[PORT_USER_BUTTON_ID].flag_pressed = true;
        }

        /* 4. Clean pending interrupt bit */
        EXTI->PR = BIT_POS_TO_MASK(buttons_arr[PORT_USER_BUTTON_ID].pin);
    }
    /* Columna 1 del teclado (segunda columna) */
    if (EXTI->PR & (1U << STM32F4_KEYBOARD_MAIN_COL_1_PIN))
    {
        _check_column_interrupt(1U);
    }
}

/**
 * @brief This function handles EXTI lines [9:5] interrupts.
 *
 */
void EXTI9_5_IRQHandler(void)
{
    port_system_systick_resume();
    /* Columna 0 del teclado (primera columna) */
    if (EXTI->PR & (1U << STM32F4_KEYBOARD_MAIN_COL_0_PIN))
    {
        _check_column_interrupt(0U);
    }

    /* Columna 3 del teclado (cuarta columna) */
    if (EXTI->PR & (1U << STM32F4_KEYBOARD_MAIN_COL_3_PIN))
    {
       _check_column_interrupt(3U);
    }
}

/**
 * @brief This function handles EXTI line 4 interrupt.
 *
 */
void EXTI4_IRQHandler(void)
{
    port_system_systick_resume();
    if (EXTI->PR & (1U << STM32F4_KEYBOARD_MAIN_COL_2_PIN))
    {
        _check_column_interrupt(2U);
    }
}

/**
 * @brief Interrupt service routine for the TIM5 timer.
 *
 */
void TIM5_IRQHandler(void)
{
    if (TIM5->SR & TIM_SR_UIF)
    {
        /* Limpiar flag de update */
        TIM5->SR &= ~TIM_SR_UIF;

        /* Avisar a la FSM de que ha vencido el tiempo de la fila */
        port_keyboard_set_row_timeout_status(PORT_KEYBOARD_MAIN_ID, true);
    }
}

/**
 * @brief Interrupt service routine for the TIM3 timer.
 *
 * This timer controls the different timing events of the Simone game. When the interrupt 
 * occurs it means that: a) the time of a color ON has passed, b) the time of the color 
 * OFF has passed, c) the time of waiting to a player keyboard pressed has passed.
 */
void TIM3_IRQHandler(void) {
    port_system_systick_resume();

    /* Comprobamos si realmente fue la interrupción de Update (UIF) del timer */
    if (TIM3->SR & TIM_SR_UIF) {
        
        /* 1. Clear the interrupt flag UIF in the status register SR. */
        TIM3->SR &= ~TIM_SR_UIF;

        /* 2. Call the function port_simone_set_timer_status() to set the flag  that indicates that the time has expired. */
        port_simone_set_timeout_status(true);
        port_simone_stop_timer();
    }
}
