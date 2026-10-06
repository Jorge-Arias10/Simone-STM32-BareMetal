/**
 * @file fsm_rgb_light.h
 * @brief Header for fsm_rgb_light.c file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

#ifndef FSM_RGB_LIGHT_SYSTEM_H_
#define FSM_RGB_LIGHT_SYSTEM_H_


/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>
#include "fsm.h"
#include "rgb_colors.h"

/* Defines and enums ----------------------------------------------------------*/
/* Enums */

enum FSM_RGB_LIGHT_SYSTEM {
  IDLE_RGB = 0,
  SET_COLOR
};

#define MAX_LEVEL_INTENSITY 100
/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the FSM of the RGB light system. 
 * 
 */
typedef struct {
    fsm_t f;
    rgb_color_t color;
    uint8_t intensity_perc;
    bool new_color;
    bool status;
    bool idle;
    uint8_t rgb_light_id;
} fsm_rgb_light_t;
/* Function prototypes and explanation -------------------------------------------------*/
/**
 * @brief Create a new RGB light FSM.
 * 
 * @param rgb_light_id 
 * @return fsm_rgb_light_t* 
 */
fsm_rgb_light_t * fsm_rgb_light_new (uint8_t rgb_light_id);

/**
 * @brief Destroy an RGB light FSM.
 * 
 * @param p_fsm 
 */
void fsm_rgb_light_destroy (fsm_rgb_light_t *p_fsm);

/**
 * @brief Set the color and intensity of the RGB light. 
 * 
 * @param p_fsm 
 * @param color 
 * @param intensity_perc 
 */
void fsm_rgb_light_set_color_intensity (fsm_rgb_light_t *p_fsm, rgb_color_t color, uint8_t intensity_perc);

/**
 * @brief Fire the RGB light FSM.
 * 
 * @param p_fsm 
 */
void fsm_rgb_light_fire (fsm_rgb_light_t *p_fsm);

/**
 * @brief Get the status of the RGB light FSM.
 * 
 * @param p_fsm 
 * @return true 
 * @return false 
 */

bool fsm_rgb_light_get_status (fsm_rgb_light_t *p_fsm);

/**
 * @brief Set the status of the RGB light FSM.
 * 
 * @param p_fsm 
 * @param pause 
 */
void fsm_rgb_light_set_status (fsm_rgb_light_t *p_fsm, bool pause);

/**
 * @brief Check if the RGB light system is active.
 * 
 * @param p_fsm 
 * @return true 
 * @return false 
 */
bool fsm_rgb_light_check_activity (fsm_rgb_light_t *p_fsm);

#endif /* FSM_RGB_LIGHT_SYSTEM_H_ */