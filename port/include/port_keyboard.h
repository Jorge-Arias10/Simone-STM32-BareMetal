/**
 * @file port_keyboard.h
 * @brief Header for the portable functions to interact with the HW of the keyboards. The functions must be implemented in the platform-specific code.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */
#ifndef PORT_KEYBOARD_H_
#define PORT_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
/* Standard C includes */

/* Defines and enums ----------------------------------------------------------*/
/**
 * @brief Keyboard identifier that represents the keyboard of the system.
 * 
 */
#define PORT_KEYBOARD_MAIN_ID 0
/**
 * @brief Keyboard scanning timeout in milliseconds.
 * 
 */
#define PORT_KEYBOARDS_TIMEOUT_MS 25
/**
 * @brief Keyboard's keys debounce time in milliseconds.
 * 
 */
#define PORT_KEYBOARD_MAIN_DEBOUNCE_TIME_MS 150

/**
 * @brief Enumeration to define the columns indexes of the keyboard. This enumeration is used to identify the columns when handling the interrupts.
 * 
 */
enum PORT_KEYBOARD_COL_IDS {
  PORT_KEYBOARD_COL_0,
  PORT_KEYBOARD_COL_1,
  PORT_KEYBOARD_COL_2,
  PORT_KEYBOARD_COL_3
};

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Configure the HW specifications of a given keyboard.
 * @param keyboard_id Keyboard ID.
 */
void port_keyboard_init(uint8_t keyboard_id);

/**
 * @brief Set the given row to high and lower the others.
 * @param keyboard_id ID of the keyboard.
 * @param row_idx Index of the row to be excited.
 */
void port_keyboard_excite_row(uint8_t keyboard_id, uint8_t row_idx);

/**
 * @brief Start the scanning of a keyboard.
 * @param keyboard_id Keyboard ID.
 */
void port_keyboard_start_scan(uint8_t keyboard_id);

/**
 * @brief Stop the scanning of a keyboard.
 * @param keyboard_id Keyboard ID.
 */
void port_keyboard_stop_scan(uint8_t keyboard_id);

/**
 * @brief Update the row to be excited (moves to the next one).
 * @param keyboard_id Keyboard ID.
 */
void port_keyboard_excite_next_row(uint8_t keyboard_id);

/**
 * @brief Return the status of the keyboard (pressed or not).
 * @param keyboard_id Keyboard ID.
 * @return true If the keyboard has been pressed.
 */
bool port_keyboard_get_key_pressed_status(uint8_t keyboard_id);

/**
 * @brief Set the status of the keyboard (pressed or not).
 * @param keyboard_id Keyboard ID.
 * @param status New status.
 */
void port_keyboard_set_key_pressed_status(uint8_t keyboard_id, bool status);

/**
 * @brief Return the status of the column timeout flag.
 * @param keyboard_id Keyboard ID.
 * @return true If the column timeout has occurred.
 */
bool port_keyboard_get_row_timeout_status(uint8_t keyboard_id);

/**
 * @brief Set the status of the row timeout flag.
 * @param keyboard_id Keyboard ID.
 * @param status New status of the row timeout flag.
 */
void port_keyboard_set_row_timeout_status(uint8_t keyboard_id, bool status);

/**
 * @brief Return the char representing the key pressed.
 * @param keyboard_id Keyboard ID.
 * @return char Key value of the key pressed.
 */
char port_keyboard_get_key_value(uint8_t keyboard_id);

/**
 * @brief Return the null key value of a given keyboard.
 * @param keyboard_id Keyboard ID.
 * @return char The invalid/null key value.
 */
char port_keyboard_get_invalid_key_value(uint8_t keyboard_id);

#endif /* PORT_KEYBOARD_H_ */
