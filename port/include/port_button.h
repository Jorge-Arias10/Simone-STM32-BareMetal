/**
 * @file port_button.h
 * @brief Header for the portable functions to interact with the HW of the buttons. The functions must be implemented in the platform-specific code.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef PORT_BUTTON_H_
/**
 * @file port_button.h
 * @author your name (you@domain.com)
 * @brief Define the header of the port_button
 * @version 0.1
 * @date 2026-03-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#define PORT_BUTTON_H_
/**
 * @file port_button.h
 * @author your name (you@domain.com)
 * @brief Our ID to work from any device.
 * @version 0.1
 * @date 2026-03-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */

/**
* @brief User button identifier that represents the button to start and config the game
 */
#define PORT_USER_BUTTON_ID 0
/**
 * @brief Button debounce time in miliseconds
 */
#define PORT_USER_BUTTON_DEBOUNCE_TIME_MS 150 //100-200, ESTO ES EL DEBOUNCE TIME

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
// Define here all the button identifiers that are used in the system

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given button
 * 
 */
void port_button_init(uint8_t button_id);

/**
 * @brief Check if the button is pressed.
 * 
 * @param button_id 
 * @return true 
 * @return false 
 */
bool port_button_get_pressed(uint8_t button_id);

#endif