/**
 * @file stm32f4_keyboard.h
 * @brief Header for stm32f4_keyboard.c file.
 * @author alumno1
 * @author alumno2
 * @date date
 */
#ifndef STM32F4_KEYBOARD_H_
#define STM32F4_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"
#include "keyboards.h"
/* Standard C includes */

/* HW dependent includes */

/**
 * @brief Structure to define the HW dependences of a keyboard status.
 * 
 */
typedef struct {
    const keyboard_t * p_keyboard;
    GPIO_TypeDef ** p_row_ports;
    uint8_t * p_row_pins;
    GPIO_TypeDef ** p_col_ports;
    uint8_t * p_col_pins;
    bool flag_key_pressed;
    bool flag_row_timeout;
    uint8_t col_idx_interrupt;
    uint8_t current_excited_row;
}  stm32f4_keyboard_hw_t;

extern stm32f4_keyboard_hw_t keyboards_arr[];

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
/**
 * @brief Keyboard GPIO port for the first row
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_0_GPIO GPIOA

/**
 * @brief Keyboard GPIO pin 0 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_0_PIN 0

/**
 * @brief Keyboard GPIO port for the second row
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_1_GPIO GPIOA

/**
 * @brief Keyboard GPIO pin 1 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_1_PIN 1

/**
 * @brief Keyboard GPIO port for the third row
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_2_GPIO GPIOA

/**
 * @brief Keyboard GPIO pin 4 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_2_PIN 4

/**
 * @brief Keyboard GPIO port for the fourth row
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_3_GPIO GPIOB

/**
 * @brief Keyboard GPIO pin 0 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_ROW_3_PIN 0

/**
 * @brief Keyboard GPIO port for the first column
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_0_GPIO GPIOA

/**
 * @brief Keyboard GPIO pin 8 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_0_PIN 8

/**
 * @brief Keyboard GPIO port for the second column
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_1_GPIO GPIOB

/**
 * @brief Keyboard GPIO pin 10 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_1_PIN 10

/**
 * @brief Keyboard GPIO port for the third column
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_2_GPIO GPIOB

/**
 * @brief Keyboard GPIO pin 4 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_2_PIN 4

/**
 * @brief Keyboard GPIO port for the fourth column
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_3_GPIO GPIOB

/**
 * @brief Keyboard GPIO pin 5 (tabla API)
 * 
 */
#define STM32F4_KEYBOARD_MAIN_COL_3_PIN 5


/* Function prototypes and explanation -------------------------------------------------*/



#endif /* STM32F4_KEYBOARD_H_ */
