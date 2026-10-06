/**
 * @file stm32f4_rgb_light.h
 * @brief Driver de bajo nivel para el control de LED RGB en STM32F4.
 * Define la configuración física de los pines y la estructura de hardware.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef STM32F4_RGB_LIGHT_SYSTEM_H_
#define STM32F4_RGB_LIGHT_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "stm32f4xx.h"

/* Defines and enums ---------------------------------------------------------*/

/** @brief Red LED GPIO port */
#define STM32F4_RGB_LIGHT_R_GPIO GPIOB
/** @brief Red LED GPIO pin */
#define STM32F4_RGB_LIGHT_R_PIN 6

/** @brief Green LED GPIO port */
#define STM32F4_RGB_LIGHT_G_GPIO GPIOB
/** @brief Green LED GPIO pin */
#define STM32F4_RGB_LIGHT_G_PIN 8

/** @brief Blue LED GPIO port */
#define STM32F4_RGB_LIGHT_B_GPIO GPIOB
/** @brief Blue LED GPIO pin */
#define STM32F4_RGB_LIGHT_B_PIN 9

/* Typedefs ------------------------------------------------------------------*/

/**
 * @brief Structure to define the HW dependencies of an RGB LED.
 */
typedef struct
{
    GPIO_TypeDef *port_r; /**< Puerto GPIO para el canal Rojo */
    uint16_t pin_r;       /**< Pin GPIO para el canal Rojo */
    
    GPIO_TypeDef *port_g; /**< Puerto GPIO para el canal Verde */
    uint16_t pin_g;       /**< Pin GPIO para el canal Verde */
    
    GPIO_TypeDef *port_b; /**< Puerto GPIO para el canal Azul */
    uint16_t pin_b;       /**< Pin GPIO para el canal Azul */
} stm32f4_rgb_light_hw_t;

/* Public variables ----------------------------------------------------------*/

/**
 * @brief Array of elements that represents the HW characteristics of the RGB LED of the RGB light systems connected to the STM32F4 platform.
 * This is an extern variable that is declared in stm32f4_rgb_light.h. It represents an array of RGB lights.
 */
extern stm32f4_rgb_light_hw_t rgb_lights_arr[];

#endif /* STM32F4_RGB_LIGHT_SYSTEM_H_ */