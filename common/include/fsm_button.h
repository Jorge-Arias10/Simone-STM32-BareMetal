/**
 * @file fsm_button.h
 * @brief Header for fsm_button.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef FSM_BUTTON_H_
/**
 * @file fsm_button.h
 * @author your name (you@domain.com)
 * @brief Define button FSM
 * @version 0.1
 * @date 2026-03-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#define FSM_BUTTON_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Other includes */
#include "fsm.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */
/**
 * @brief Our enum for our FSM button.
 * 
 */
enum  FSM_BUTTON {
  BUTTON_RELEASED = 0,
  BUTTON_RELEASED_WAIT,
  BUTTON_PRESSED,
  BUTTON_PRESSED_WAIT
};


/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Our structure for our button.
 * 
 */
typedef struct {
    fsm_t f;                  // Máquina de estados genérica
    uint32_t debounce_time_ms;   // Tiempo para el antirrebote en ms
    uint32_t next_timeout;       // Próximo tiempo de vencimiento
    uint32_t tick_pressed;       // Tick de reloj cuando se presionó
    uint32_t duration;           // Duración de la pulsación
    uint32_t button_id;          // Identificador del botón
} fsm_button_t;

/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Create a new button FSM
 * 
 * @param debounce_time_ms 
 * @param button_id 
 * @return fsm_button_t* 
 */
fsm_button_t * fsm_button_new (uint32_t debounce_time_ms, uint8_t button_id);
 
/**
 * @brief Destroy a button FSM
 * 
 * @param p_fsm 
 */
void fsm_button_destroy (fsm_button_t *p_fsm);
 
/**
 * @brief Fire the button FSM
 * 
 * @param p_fsm 
 */
void fsm_button_fire (fsm_button_t *p_fsm);
 
/**
 * @brief Return the duration of the last button press
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_duration (fsm_button_t *p_fsm);
 

/**
 * @brief Reset the duration of the last button press
 * 
 * @param p_fsm 
 */
void fsm_button_reset_duration (fsm_button_t *p_fsm);
 
/**
 * @brief Get the debounce time of the button FSM
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_debounce_time_ms (fsm_button_t *p_fsm);
 
/**
 * @brief Check if the button FSM is active, or not
 * 
 * @param p_fsm 
 * @return true 
 * @return false 
 */
bool fsm_button_check_activity (fsm_button_t *p_fsm);


#endif