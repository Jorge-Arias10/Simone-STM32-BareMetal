/**
 * @file fsm_keyboard.h
 * @brief Header for fsm_keyboard.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef FSM_KEYBOARD_H_
#define FSM_KEYBOARD_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Project includes */
#include "fsm.h"
#include "port_keyboard.h"

/* Defines and enums ----------------------------------------------------------*/

/**
 * @brief States of the keyboard finite state machine.
 *
 */
typedef enum {
    KEYBOARD_RELEASED_WAIT_ROW = 0, /*!< Initial state. Waits for row timeout or key press. */
    KEYBOARD_PRESSED_WAIT,          /*!< A key press has been detected. Waiting for debounce time. */
    KEYBOARD_PRESSED,               /*!< Key is considered pressed. Waiting for release. */
    KEYBOARD_RELEASED_WAIT          /*!< Key release detected. Waiting for debounce time. */
} FSM_KEYBOARD;

/* Typedefs --------------------------------------------------------------------*/

/**
 * @brief Structure to define a keyboard FSM.
 *
 */
typedef struct {
    fsm_t f;                 /*!< Base FSM structure. Must be the first field. */
    uint8_t keyboard_id;     /*!< Identifier of the keyboard handled by this FSM. */

    uint32_t debounce_time_ms; /*!< Debounce time in milliseconds (SW anti-bounce) */
    uint32_t next_timeout;     /*!< Tick when next timeout will expire */

    uint32_t tick_pressed;   /*!< Tick when the key press was detected. */
    char key_value;
             /*!< Last key value detected by the FSM. */
} fsm_keyboard_t;

/* Function prototypes and explanation -------------------------------------------------*/

/**
 * @brief Initialize a keyboard FSM.
 *
 * This function initializes both the FSM internal data and the keyboard PORT layer.
 *
 * @param p_fsm_keyboard Pointer to the keyboard FSM structure.
 * @param keyboard_id Identifier of the keyboard handled by the FSM.
 */
void fsm_keyboard_init(fsm_keyboard_t *p_fsm_keyboard, uint32_t debounce_time, uint8_t keyboard_id);

/**
 * @brief Create a new keyboard FSM in dynamic memory.
 *
 * @param keyboard_id Identifier of the keyboard handled by the FSM.
 * @return fsm_keyboard_t* Pointer to the created FSM.
 */
fsm_keyboard_t *fsm_keyboard_new(uint32_t debounce_time, uint8_t keyboard_id);

/**
 * @brief Start scanning the keyboard.
 *
 * @param p_fsm_keyboard Pointer to the FSM.
 */
void fsm_keyboard_start_scan(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Stop scanning the keyboard.
 *
 * @param p_fsm_keyboard Pointer to the FSM.
 */
void fsm_keyboard_stop_scan(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Get the last key value detected by the keyboard FSM.
 *
 * @param fsm_keyboard_t Pointer to the FSM.
 * @return char Last detected key. If no new key is available, returns the invalid key value.
 */
char fsm_keyboard_get_key_value(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Check if the stored key is valid (different from invalid key).
 *
 * @param fsm_keyboard_t Pointer to the FSM.
 * @return true if valid key
 * @return false otherwise
 */
bool fsm_keyboard_get_is_valid_key(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Reset the stored key value to the invalid key value.
 *
 * @param fsm_keyboard_t Pointer to the FSM.
 */
void fsm_keyboard_reset_key_value(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Execute one iteration of the FSM.
 *
 * @param p_fsm_keyboard Pointer to the FSM.
 */
void fsm_keyboard_fire(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Check if the keyboard FSM is active, or not.
 * 
 * @param p_fsm_keyboard 
 * @return true 
 * @return false 
 */
bool fsm_keyboard_check_activity(fsm_keyboard_t *p_fsm_keyboard);

/**
 * @brief Destroy a keyboard FSM.
 * 
 * @param p_fsm_keyboard 
 */
void fsm_keyboard_destroy(fsm_keyboard_t *p_fsm_keyboard);

#endif /* FSM_KEYBOARD_H_ */