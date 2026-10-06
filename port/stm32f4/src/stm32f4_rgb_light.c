/**
 * @file stm32f4_rgb_light.c
 * @brief Portable functions to interact with the RGB light system FSM library. All portable functions must be implemented in this file.
 * @author alumno1
 * @author alumno2
 * @date 2026-01-01
 */

/* Standard C includes */
#include <stdio.h>

/* HW dependent includes */
#include "port_rgb_light.h"
#include "port_system.h"
#include "stm32f4_system.h"
#include "stm32f4_rgb_light.h"

/* Microcontroller dependent includes */

/* Defines --------------------------------------------------------------------*/

/* Typedefs --------------------------------------------------------------------*/

/* Global variables ------------------------------------------------------------*/

/**
 * @brief Array of elements that represents the HW characteristics of the RGB LED of the RGB light systems connected to the STM32F4 platform.
 * 
 */
stm32f4_rgb_light_hw_t rgb_lights_arr[] = {
    {
        .port_r = STM32F4_RGB_LIGHT_R_GPIO,
        .pin_r  = STM32F4_RGB_LIGHT_R_PIN,
        .port_g = STM32F4_RGB_LIGHT_G_GPIO,
        .pin_g  = STM32F4_RGB_LIGHT_G_PIN,
        .port_b = STM32F4_RGB_LIGHT_B_GPIO,
        .pin_b  = STM32F4_RGB_LIGHT_B_PIN
    }
};

/* Private functions -----------------------------------------------------------*/

/**
 * @brief Get the RGB light struct with the given ID.
 */
stm32f4_rgb_light_hw_t *_stm32f4_rgb_light_get(uint8_t rgb_light_id)
{
    // Return the pointer to the rgb light with the given ID. If the ID is not valid, return NULL.
    if (rgb_light_id == PORT_RGB_LIGHT_ID) {
        return &rgb_lights_arr[0];
    }
    return NULL;
}

/**
 * @brief Configure the timer that controls the PWM of each one of the RGB LEDs.
 */
void _timer_pwm_config(uint8_t rgb_light_id)
{
    if (rgb_light_id == PORT_RGB_LIGHT_ID) {
        /* TODO alumnos: PWM timer setup */
        /* 1. Enable the clock source of the timer (del manual de usuario, TIM4 está en APB1, página 59) */
        RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;

        /* 2. Disable the counter (register CR1) and enable the autoreload preload (bit ARPE) */
        TIM4->CR1 &= ~TIM_CR1_CEN;
        TIM4->CR1 |= TIM_CR1_ARPE;

        /* 3. Reset the counter (register CNT), set the autoreload value (register ARR) and the prescaler (register PSC) for a frequency of 50 Hz. */
        TIM4->CNT = 0;
        TIM4->PSC = 16 - 1;      /* 16 MHz / 16 = 1 MHz (Tick de 1 microseg) */
        TIM4->ARR = 20000 - 1;   /* 1 MHz / 20000 = 50 Hz (Periodo exacto de 20 ms)*/

        /* PWM mode configuration */
        /* 5. Disable the output compare (register CCER) for each one of the corresponding channels (CH1, CH3, CH4). */
        TIM4->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC3E | TIM_CCER_CC4E);

        /* 6. Clear the P and NP bits (CCxP and CCxNP) of the output compare register (CCER) */
        TIM4->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP | 
                        TIM_CCER_CC3P | TIM_CCER_CC3NP | 
                        TIM_CCER_CC4P | TIM_CCER_CC4NP);

        /* 7. Set both (i) mode PWM 1, and (ii) enable preload (register CCMRx) */
        /* CH1 -> CCMR1 */ //configura canales 1 y 2
        // limpieza previa: &= ~... (invierte máscara y pone AND: pone a 0)
        TIM4->CCMR1 &= ~TIM_CCMR1_OC1M;
        TIM4->CCMR1 |= (0x6 << TIM_CCMR1_OC1M_Pos); /* PWM Mode 1 (110 en binario) => página 558 del manual */
        TIM4->CCMR1 |= TIM_CCMR1_OC1PE;

        /* CH3 & CH4 -> CCMR2 */ //configura canales 3 y 4
        TIM4->CCMR2 &= ~(TIM_CCMR2_OC3M | TIM_CCMR2_OC4M);
        TIM4->CCMR2 |= (0x6 << TIM_CCMR2_OC3M_Pos) | (0x6 << TIM_CCMR2_OC4M_Pos); /* PWM Mode 1 */
        TIM4->CCMR2 |= (TIM_CCMR2_OC3PE | TIM_CCMR2_OC4PE);

        /* 8. Generate an update event (register EGR) by setting the UG bit. */
        TIM4->EGR |= TIM_EGR_UG;
    }
}

/* Public functions -----------------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given RGB light.
 * 
 * @param rgb_light_id 
 */
void port_rgb_light_init(uint8_t rgb_light_id) {
    /* 1. Retrieve the RGB light configuration struct calling _stm32f4_rgb_light_get() */
    stm32f4_rgb_light_hw_t *hw = _stm32f4_rgb_light_get(rgb_light_id);

    if (hw != NULL) {
        /* 2. Call function stm32f4_system_gpio_config() with the right arguments to configure each RGB LED as in alternate mode and no pull up neither pull down connection. */
        /* es STM32F4_GPIO_MODE_AF porque va el modo general del pin (enstm32f4_system.h) */
        stm32f4_system_gpio_config(hw->port_r, hw->pin_r, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
        stm32f4_system_gpio_config(hw->port_g, hw->pin_g, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
        stm32f4_system_gpio_config(hw->port_b, hw->pin_b, STM32F4_GPIO_MODE_AF, STM32F4_GPIO_PUPDR_NOPULL);
        /* 3. Call function stm32f4_system_gpio_config_alternate() with the right arguments to configure the alternate function of the each RGB LED. */
        /* es STM32F4_AF2 porque en la datasheet TIM4 se corresponde con esa (y esta definido en stm32f4_system.h), y pide el num de funcion alternativa especifica */
        stm32f4_system_gpio_config_alternate(hw->port_r, hw-> pin_r, STM32F4_AF2);
        stm32f4_system_gpio_config_alternate(hw->port_g, hw-> pin_g, STM32F4_AF2);
        stm32f4_system_gpio_config_alternate(hw->port_b, hw-> pin_b, STM32F4_AF2);
        /* 4. Call function _timer_pwm_config() to configure the timer and the PWM signal of the RGB light. */
        _timer_pwm_config(rgb_light_id);
        /* 5. Call function port_rgb_light_set_rgb() to set the RGB LED to off. */
        port_rgb_light_set_rgb(rgb_light_id, color_off);
    }

}

/**
 * @brief Set the Capture/Compare register values for each channel of the RGB LED given a color.
 * 
 * @param rgb_light_id 
 * @param color 
 */
void port_rgb_light_set_rgb	(uint8_t rgb_light_id, rgb_color_t color) {
    /* Write the code in a conditional statement to check the rgb_light_id. */
    if (rgb_light_id == PORT_RGB_LIGHT_ID) {
        /* [Flujograma] 1. Retrieve individual RGB levels (r, g, b) */
        uint8_t r = color.r;
        uint8_t g = color.g;
        uint8_t b = color.b;

        /* [Flujograma] 2. Disable the timer */
        TIM4->CR1 &= ~TIM_CR1_CEN;

        /* [Flujograma] 3. Check if r=g=b=0 */
        if (r == 0 && g == 0 && b == 0) {
            /* Disable the capture/compare register for all the channels */
            TIM4->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC3E | TIM_CCER_CC4E);
        } else {
            /* [Flujograma] For the channel of the red LED (CH1): */
            if (r == 0) {
                TIM4->CCER &= ~TIM_CCER_CC1E; /* Disable CH1 */
            } else {
                /* Set the duty cycle in compare register */
                TIM4->CCR1 = (uint32_t)(r * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC1E;  /* Enable CH1 */
            }
            
            /* [Flujograma] For the channel of the green LED (CH3): */
            if (g == 0) {
                TIM4->CCER &= ~TIM_CCER_CC3E; /* Disable CH3 */
            } else {
                /* Set the duty cycle in compare register */
                TIM4->CCR3 = (uint32_t)(g * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC3E;  /* Enable CH3 */
            }

            /* [Flujograma] For the channel of the blue LED (CH4): */
            if (b == 0) {
                TIM4->CCER &= ~TIM_CCER_CC4E; /* Disable CH4 */
            } else {
                /* Set the duty cycle in compare register */
                TIM4->CCR4 = (uint32_t)(b * (TIM4->ARR + 1)) / COLOR_RGB_MAX_VALUE;
                TIM4->CCER |= TIM_CCER_CC4E;  /* Enable CH4 */

                /* [Flujograma] Set the UG bit in the EGR register */
                TIM4->EGR |= TIM_EGR_UG;
        
                /* [Flujograma] Enable the timer */
                TIM4->CR1 |= TIM_CR1_CEN;
            }
        } 
    }
}
