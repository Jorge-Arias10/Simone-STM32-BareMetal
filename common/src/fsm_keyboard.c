/**
 * @file fsm_keyboard.c
 * @brief Keyboard sensor FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* HW dependent includes */

/* Project includes */
#include "fsm_keyboard.h"
#include "port_keyboard.h"
#include "port_system.h"
#include "fsm.h"

/* Typedefs --------------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/

/* State machine input or transition functions */

/**
 * @brief Check if the time to activate a new row and scan columns has passed.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_row_timeout(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;
    return port_keyboard_get_row_timeout_status(p->keyboard_id);
}

/**
 * @brie  Check if the keyboard has been pressed.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_keyboard_pressed(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;
    return port_keyboard_get_key_pressed_status(p->keyboard_id);
}

/**
 * @brief Check if the keyboard has been released.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_keyboard_released(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;
    return !port_keyboard_get_key_pressed_status(p->keyboard_id);
}

/**
 * @brief Check if the debounce-time has passed.
 *
 * @param p_this
 * @return true
 * @return false
 */
static bool check_timeout(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;
    return (port_system_get_millis() > p->next_timeout);
}

/* State machine output or action functions */
/**
 * @brief Clean the row timeout flag and update the row to be excited.
 *
 * @param p_this
 */
static void do_excite_next_row(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;

    port_keyboard_set_row_timeout_status(p->keyboard_id, false);
    port_keyboard_excite_next_row(p->keyboard_id);
}

/**
 * @brief Store the system tick when the keyboard was pressed.
 *
 * @param p_this
 */
static void do_store_tick_pressed(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;

    p->tick_pressed = port_system_get_millis();
    p->next_timeout = p->tick_pressed + p->debounce_time_ms;
}

/**
 * @brief Store the key value of the keyboard press.
 *
 * @param p_this
 */
static void do_set_key_value(fsm_t *p_this)
{
    fsm_keyboard_t *p = (fsm_keyboard_t *)p_this;

    p->key_value = port_keyboard_get_key_value(p->keyboard_id);
    p->next_timeout = port_system_get_millis() + p->debounce_time_ms;
}

/* Transition table */
/**
 * @brief Array representing the transitions table of the FSM keyboard.
 *
 */
static fsm_trans_t fsm_trans_keyboard[] = {
    {KEYBOARD_RELEASED_WAIT_ROW, check_row_timeout, KEYBOARD_RELEASED_WAIT_ROW, do_excite_next_row},
    {KEYBOARD_RELEASED_WAIT_ROW, check_keyboard_pressed, KEYBOARD_PRESSED_WAIT, do_store_tick_pressed},
    {KEYBOARD_PRESSED_WAIT, check_timeout, KEYBOARD_PRESSED, NULL},
    {KEYBOARD_PRESSED, check_keyboard_released, KEYBOARD_RELEASED_WAIT, do_set_key_value},
    {KEYBOARD_RELEASED_WAIT, check_timeout, KEYBOARD_RELEASED_WAIT_ROW, NULL},
    {-1, NULL, -1, NULL}};

/* Other auxiliary functions */
/**
 * @brief Initialize a keyboard FSM.
 *
 * @param p_fsm_keyboard
 * @param keyboard_id
 */
void fsm_keyboard_init(fsm_keyboard_t *p_fsm_keyboard, uint32_t debounce_time, uint8_t keyboard_id)
{
    if (p_fsm_keyboard == NULL)
    {
        return;
    }

    port_keyboard_init(keyboard_id);

    fsm_init(&p_fsm_keyboard->f, fsm_trans_keyboard);

    p_fsm_keyboard->keyboard_id = keyboard_id;
    p_fsm_keyboard->debounce_time_ms = debounce_time;
    p_fsm_keyboard->next_timeout = 0U;
    p_fsm_keyboard->tick_pressed = 0U;
    p_fsm_keyboard->key_value = port_keyboard_get_invalid_key_value(keyboard_id);
}

/* Public functions -----------------------------------------------------------*/
/**
 * @brief Create a new keyboard FSM.
 *
 * @param keyboard_id
 * @return fsm_keyboard_t*
 */
fsm_keyboard_t *fsm_keyboard_new(uint32_t debounce_time_ms, uint8_t keyboard_id)
{
    fsm_keyboard_t *p_fsm_keyboard = malloc(sizeof(fsm_keyboard_t));
    if (p_fsm_keyboard != NULL)
    {
        fsm_keyboard_init(p_fsm_keyboard, debounce_time_ms, keyboard_id);
    }
    return p_fsm_keyboard;
}

/**
 * @brief Start keyboard scanning
 */
void fsm_keyboard_start_scan(fsm_keyboard_t *p_fsm_keyboard)
{
    if (p_fsm_keyboard == NULL)
        return;

    port_keyboard_start_scan(p_fsm_keyboard->keyboard_id);
}

/**
 * @brief Stop keyboard scanning
 */
void fsm_keyboard_stop_scan(fsm_keyboard_t *p_fsm_keyboard)
{
    if (p_fsm_keyboard == NULL)
        return;

    port_keyboard_stop_scan(p_fsm_keyboard->keyboard_id);
}

/**
 * @brief Return the key pressed of the last keyboard press.
 *
 * @param p_fsm
 * @return char
 */
char fsm_keyboard_get_key_value(fsm_keyboard_t *p_fsm_keyboard)
{
    return p_fsm_keyboard->key_value;
}

/**
 * @brief Check if the stored key is valid
 */
bool fsm_keyboard_get_is_valid_key(fsm_keyboard_t *p_fsm_keyboard)
{
    return (p_fsm_keyboard->key_value != port_keyboard_get_invalid_key_value(p_fsm_keyboard->keyboard_id));
}

/**
 * @brief Reset the key pressed of the last keyboard pressed.
 *
 * @param p_fsm
 */
void fsm_keyboard_reset_key_value(fsm_keyboard_t *p_fsm_keyboard)
{
    p_fsm_keyboard->key_value = port_keyboard_get_invalid_key_value(p_fsm_keyboard->keyboard_id);
}

/**
 * @brief Execute FSM
 */
void fsm_keyboard_(fsm_keyboard_t *p_fsm_keyboard)
{
    if (p_fsm_keyboard == NULL)
        return;

    fsm_fire(&p_fsm_keyboard->f);
}

void fsm_keyboard_fire(fsm_keyboard_t *p_fsm)
{
    fsm_fire(&p_fsm->f); // Is it also possible to it in this way: fsm_fire((fsm_t *)p_fsm);
}

/** V4 */
/**
 * @brief Check if the keyboard FSM is active, or not.
 * 
 * @param p_fsm 
 * @return bool
 */
bool fsm_keyboard_check_activity(fsm_keyboard_t * p_fsm) {
    return false;
}

/**
 * @brief This function destroys the keyboard FSM and frees the memory.
 * 
 * @param p_fsm_keyboard 
 */
void fsm_keyboard_destroy(fsm_keyboard_t *p_fsm_keyboard)
{
    if (p_fsm_keyboard != NULL) {
        free(p_fsm_keyboard);
    }
}