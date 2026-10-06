/**
 * @file stm32f4_button.c
 * @brief Portable functions to interact with the button FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

/* HW dependent includes */
#include "port_button.h" // Used to get general information about the buttons (ID, etc.)
#include "port_system.h" // Used to get the system tick


/* Microcontroller dependent includes */
// TO-DO alumnos: include the necessary files to interact with the GPIOs
#include <stdio.h>
#include "stm32f4_system.h"
#include "stm32f4_button.h"

/* Global variables ------------------------------------------------------------*/
/**
 * @brief Array of elements that represents the HW characteristics of the buttons connected to the STM32F4 platform.
 * This is an **extern** variable that is declared in 'stm32f4_button.h'. It represents an array of hardware buttons.
 * @hideinitializer
 */
stm32f4_button_hw_t buttons_arr[] = { 
    [PORT_USER_BUTTON_ID] = {
        .p_port = STM32F4_USER_BUTTON_GPIO, 
        .pin = STM32F4_USER_BUTTON_PIN, 
        .pupd_mode = STM32F4_GPIO_PUPDR_NOPULL},
};

/* Private functions ----------------------------------------------------------*/
/**
 * @brief Get the button status struct with the given ID.
 *
 * @param button_id Button ID.
 *
 * @return Pointer to the button state struct.
 * @return NULL If the button ID is not valid.
 */
stm32f4_button_hw_t *_stm32f4_button_get(uint8_t button_id)
{
    // Return the pointer to the button with the given ID. If the ID is not valid, return NULL.
    if (button_id < sizeof(buttons_arr) / sizeof(buttons_arr[0]))
    {
        return &buttons_arr[button_id];
    }
    else
    {
        return NULL;
    }
}

/* Public functions -----------------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given buttons.
 * 
 * @param button_id 
 */
void port_button_init(uint8_t button_id)
{
    // Retrieve the button struct using the private function and the button ID
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    /* TO-DO alumnos */
    stm32f4_system_gpio_config(p_button->p_port, p_button->pin, STM32F4_GPIO_MODE_IN,p_button->pupd_mode);
    stm32f4_system_gpio_config_exti(p_button->p_port, p_button->pin, STM32F4_TRIGGER_BOTH_EDGE | STM32F4_TRIGGER_ENABLE_INTERR_REQ);
    stm32f4_system_gpio_exti_enable(p_button->pin, 1, 0);
}

/**
 * @brief Return the status of the button (pressed or not).
 * 
 * @param button_id 
 * @return true 
 * @return false 
 */
bool port_button_get_pressed(uint8_t button_id)
{
    stm32f4_button_hw_t *p_button = _stm32f4_button_get(button_id);
    return p_button->flag_pressed;
}
