/**
 * @file stm32f4_simone.h
 * @brief Header for stm32f4_simone.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef STM32F4_SIMONE_H_
#define STM32F4_SIMONE_H_
/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* HW dependent includes */
#include "stm32f4xx.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a Simone's timer status.
 */
typedef struct
{
    bool flag_timer_timeout; /*!< Flag to indicate that the timer passed */
} stm32f4_simone_hw_t;

/* Global variables */
/**
 * @brief Element that represents the HW characteristics of the Simone game connected to the STM32F4 platform to manage the timers.
 *
 * This is an **extern** variable that is defined in `stm32f4_simone.c`. It represents the hardware of Simone.
 */
extern stm32f4_simone_hw_t simone_hw;


bool port_simone_get_timeout_status(void);

#endif /* STM32F4_SIMONE_H_ */