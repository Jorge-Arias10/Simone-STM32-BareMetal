/**
 * @file stm32f4_keyboard.c
 * @brief Portable functions to interact with the keyboard FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date date
 */

/* Standard C includes */
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "port_keyboard.h"
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_keyboard.h"
#include "keyboards.h"
/* HW dependent includes */

/* Microcontroller dependent includes */

/* Typedefs --------------------------------------------------------------------*/

/* Global variables */

/* Static arrays for main keyboard (pointed by the double pointers in the struct) */

/**
 * @brief Array of GPIO ports for the rows of the main keyboard.
 *
 */
static GPIO_TypeDef *keyboard_main_row_ports[] = {
    STM32F4_KEYBOARD_MAIN_ROW_0_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_1_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_2_GPIO,
    STM32F4_KEYBOARD_MAIN_ROW_3_GPIO};

/**
 * @brief Array of GPIO pins for the rows of the main keyboard.
 *
 */
static uint8_t keyboard_main_row_pins[] = {
    STM32F4_KEYBOARD_MAIN_ROW_0_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_1_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_2_PIN,
    STM32F4_KEYBOARD_MAIN_ROW_3_PIN};

/**
 * @brief Array of GPIO ports for the columns of the main keyboard.
 *
 */
static GPIO_TypeDef *keyboard_main_col_ports[] = {
    STM32F4_KEYBOARD_MAIN_COL_0_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_1_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_2_GPIO,
    STM32F4_KEYBOARD_MAIN_COL_3_GPIO};

/**
 * @brief Array of GPIO pins for the columns of the main keyboard.
 *
 */
static uint8_t keyboard_main_col_pins[] = {
    STM32F4_KEYBOARD_MAIN_COL_0_PIN,
    STM32F4_KEYBOARD_MAIN_COL_1_PIN,
    STM32F4_KEYBOARD_MAIN_COL_2_PIN,
    STM32F4_KEYBOARD_MAIN_COL_3_PIN};

/**
 * @brief Array of elements that represents the HW characteristics of the keyboards connected to the STM32F4 platform
 *
 */
stm32f4_keyboard_hw_t keyboards_arr[] = {
    {
        .p_keyboard = &standard_keyboard,
        .p_row_ports = keyboard_main_row_ports,
        .p_row_pins = keyboard_main_row_pins,
        .p_col_ports = keyboard_main_col_ports,
        .p_col_pins = keyboard_main_col_pins,
    }};

/* Private functions ----------------------------------------------------------*/
/**
 * @brief Get the keyboard struct with the given ID.
 *
 * @param keyboard_id
 * @return stm32f4_keyboard_hw_t*
 */
stm32f4_keyboard_hw_t *_stm32f4_keyboard_get(uint8_t keyboard_id)
{
    // Return the pointer to the button with the given ID. If the ID is not valid, return NULL.
    if (keyboard_id < sizeof(keyboards_arr) / sizeof(keyboards_arr[0]))
    {
        return &keyboards_arr[keyboard_id];
    }
    else
    {
        return NULL;
    }
}


static void _timer_scan_column_config(void)
{
    uint32_t timer_clk_hz = SystemCoreClock;
    uint32_t tick_hz = 1000000U; /* 1 MHz */
    uint32_t psc = (uint32_t)round(((double)timer_clk_hz / (double)tick_hz) - 1.0);
    uint32_t arr = (uint32_t)round(((double)PORT_KEYBOARDS_TIMEOUT_MS * (double)tick_hz / 1000.0) - 1.0);

    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    TIM5->CR1 &= ~TIM_CR1_CEN;
    TIM5->CR1 &= ~TIM_CR1_DIR;
    TIM5->CR1 |= TIM_CR1_ARPE;

    TIM5->PSC = psc;
    TIM5->ARR = arr;
    TIM5->CNT = 0U;

    TIM5->EGR = TIM_EGR_UG;
    TIM5->SR &= ~TIM_SR_UIF;
    TIM5->DIER |= TIM_DIER_UIE;

    NVIC_SetPriority(
        TIM5_IRQn,
        NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2U, 0U));

    /* la IRQ se habilita al empezar el scan, asiq ue mejor ponerla aqui */
    NVIC_DisableIRQ(TIM5_IRQn);
}

/* Public functions -----------------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given keyboard.
 *
 * @param keyboard_id
 */
void port_keyboard_init(uint8_t keyboard_id)
{
    /* Get the keyboard sensor */
    /**
     * @brief Get the keyboard struct with the given ID.
     *
     */
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
        return;

    /* TO-DO alumnos: */

    /* Rows configuration */
    // sacado de stm32f4_system.h
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_config(p_keyboard->p_row_ports[i], p_keyboard->p_row_pins[i], STM32F4_GPIO_MODE_OUT, STM32F4_GPIO_PUPDR_NOPULL);

        stm32f4_system_gpio_write(
            p_keyboard->p_row_ports[i],
            p_keyboard->p_row_pins[i],
            false);
    }
    /* Columns configuration */
    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_cols; i++)
    {
        // Configurar como entrada con pull-down
        stm32f4_system_gpio_config(p_keyboard->p_col_ports[i], p_keyboard->p_col_pins[i], STM32F4_GPIO_MODE_IN, STM32F4_GPIO_PUPDR_PULLDOWN);

        // Configurar interrupción en ambos flancos
        stm32f4_system_gpio_config_exti(p_keyboard->p_col_ports[i], p_keyboard->p_col_pins[i], STM32F4_TRIGGER_BOTH_EDGE | STM32F4_TRIGGER_ENABLE_INTERR_REQ);

        // Habilitar con prioridad 1, subprioridad 1
        stm32f4_system_gpio_exti_enable(p_keyboard->p_col_pins[i], 1, 1);
    }
    /* Clean/set all configurations */
    p_keyboard->flag_key_pressed = false;
    p_keyboard->flag_row_timeout = false;
    p_keyboard->current_excited_row = -1;
    p_keyboard->col_idx_interrupt = 0;

    /* Configure timer */
    _timer_scan_column_config();
}

// funciones de estado

/**
 * @brief Return the status of the keyboard (pressed or not).
 *
 * @param keyboard_id
 * @return true
 * @return false
 */
bool port_keyboard_get_key_pressed_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return false;
    }

    return p_keyboard->flag_key_pressed;
}

/**
 * @brief Set the status of the keyboard (pressed or not).
 *
 * @param keyboard_id
 * @param status
 */
void port_keyboard_set_key_pressed_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    p_keyboard->flag_key_pressed = status;
}

/**
 * @brief Return the char representing the key pressed of a given keyboard based on its row that is being excited. This assumes that the matrix of chars is flattened (i.e., it is not a 2D array, but all rows are in a single array), thus it is necessary to calculate only one index.
 *
 * @param keyboard_id
 * @return true
 * @return false
 */
bool port_keyboard_get_row_timeout_status(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return false;
    }

    return p_keyboard->flag_row_timeout;
}

/**
 * @brief Set the status of the row timeout flag.
 *
 * @param keyboard_id
 * @param status
 */
void port_keyboard_set_row_timeout_status(uint8_t keyboard_id, bool status)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    p_keyboard->flag_row_timeout = status;
}

// OBTENER VALOR TECLA

/**
 * @brief Return the char representing the key pressed of a given keyboard based on its row that is being excited. This assumes that the matrix of chars is flattened (i.e., it is not a 2D array, but all rows are in a single array), thus it is necessary to calculate only one index.
 *
 * @param keyboard_id
 * @return char
 */
char port_keyboard_get_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return '\0';
    }

    if (p_keyboard->current_excited_row >= p_keyboard->p_keyboard->num_rows)
    {
        return p_keyboard->p_keyboard->null_key;
    }

    if (p_keyboard->col_idx_interrupt >= p_keyboard->p_keyboard->num_cols)
    {
        return p_keyboard->p_keyboard->null_key;
    }

    uint8_t index = (p_keyboard->current_excited_row * p_keyboard->p_keyboard->num_cols) + p_keyboard->col_idx_interrupt;

    return p_keyboard->p_keyboard->keys[index];
}

/**
 * @brief Return the null key value of a given keyboard.
 *
 * @param keyboard_id
 * @return char
 */
char port_keyboard_get_invalid_key_value(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return '\0';
    }

    return p_keyboard->p_keyboard->null_key;
}

/**
 * @brief Start the scanning of a keyboard.
 *
 * @param keyboard_id
 */
void port_keyboard_start_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    p_keyboard->flag_row_timeout = false;
    p_keyboard->flag_key_pressed = false;
    p_keyboard->col_idx_interrupt = 0xFFU;

    p_keyboard->current_excited_row = 0U;
    port_keyboard_excite_row(keyboard_id, 0U);

    TIM5->CNT = 0U;
    TIM5->SR &= ~TIM_SR_UIF;

    NVIC_EnableIRQ(TIM5_IRQn);
    TIM5->CR1 |= TIM_CR1_CEN;
}

/**
 * @brief Stop the scanning of a keyboard.
 *
 * @param keyboard_id
 */
void port_keyboard_stop_scan(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    TIM5->CR1 &= ~TIM_CR1_CEN;
    NVIC_DisableIRQ(TIM5_IRQn);

    TIM5->CNT = 0U;
    TIM5->SR &= ~TIM_SR_UIF;

    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_write(
            p_keyboard->p_row_ports[i],
            p_keyboard->p_row_pins[i],
            false);
    }

    p_keyboard->current_excited_row = 0xFFU;
    p_keyboard->col_idx_interrupt = 0xFFU;
    p_keyboard->flag_row_timeout = false;
    p_keyboard->flag_key_pressed = false;
}


/**
 * @brief Set the given row to high and lower the others.
 *
 * @param keyboard_id
 * @param row_idx
 */
void port_keyboard_excite_row(uint8_t keyboard_id, uint8_t row_idx)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    if (row_idx >= p_keyboard->p_keyboard->num_rows)
    {
        return;
    }

    for (uint8_t i = 0; i < p_keyboard->p_keyboard->num_rows; i++)
    {
        stm32f4_system_gpio_write(
            p_keyboard->p_row_ports[i],
            p_keyboard->p_row_pins[i],
            (i == row_idx));
    }
}

/**
 * @brief Update the row to be excited.
 *
 * @param keyboard_id
 */
void port_keyboard_excite_next_row(uint8_t keyboard_id)
{
    stm32f4_keyboard_hw_t *p_keyboard = _stm32f4_keyboard_get(keyboard_id);

    if (p_keyboard == NULL)
    {
        return;
    }

    if (p_keyboard->current_excited_row == 0xFFU)
    {
        p_keyboard->current_excited_row = 0U;
    }
    else
    {
        p_keyboard->current_excited_row =
            (p_keyboard->current_excited_row + 1U) % p_keyboard->p_keyboard->num_rows;
    }

    port_keyboard_excite_row(keyboard_id, p_keyboard->current_excited_row);
}