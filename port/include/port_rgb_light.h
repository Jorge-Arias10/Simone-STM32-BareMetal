/**
 * @file port_rgb_light.h
 * @brief Header for the portable functions to interact with the HW of the RGB light system. The functions must be implemented in the platform-specific code.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef PORT_RGB_LIGHT_SYSTEM_H_
#define PORT_RGB_LIGHT_SYSTEM_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include "rgb_colors.h"

/* Typedefs --------------------------------------------------------------------*/

/* Defines and enums ----------------------------------------------------------*/
/* Defines */

/**
 * @brief Identificador del RGB light trasero.
 * Si es el único RGB light del sistema, se le asigna el 0.
 */
#define PORT_RGB_LIGHT_ID 0

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Configure the HW specifications of a given RGB light.
 * @param rgb_light_id Identificador del RGB light que se va a inicializar.
 */
void port_rgb_light_init(uint8_t rgb_light_id);

/**
 * @brief Set the Capture/Compare register values for each channel of the RGB LED given a color.
 * @param rgb_light_id Identificador del RGB light al que se le aplicará el color.
 * @param color Color a configurar, definido por el tipo rgb_color_t.
 */
void port_rgb_light_set_rgb(uint8_t rgb_light_id, rgb_color_t color);

#endif /* PORT_RGB_LIGHT_SYSTEM_H_ */