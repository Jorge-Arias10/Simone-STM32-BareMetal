/**
 * @file fsm_rgb_light.c
 * @brief RGB light system FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
/* HW dependent includes */
#include "port_rgb_light.h"
#include "port_system.h"
#include "fsm.h"
#include "fsm_rgb_light.h"
#include "rgb_colors.h"
/* Project includes */

/* Typedefs --------------------------------------------------------------------*/

/* Private functions -----------------------------------------------------------*/
/**
 * @brief Apply correction to an RGB color based on the intensity value.
 * 
 * @param p_color 
 * @param intensity_perc 
 */
void _correct_rgb_light_levels (rgb_color_t *p_color, uint8_t intensity_perc) {
    /* 1. Scale each channel by the intensity given using floats. */
    /* Se divide entre 100 para ponerlo como factor (de 0.0 a 1.0)*/
    float factor = intensity_perc / 100.0f;

    /*2. El +0.5 es para redondear en vez de truncar*/
    p_color->r = (uint8_t)((p_color->r * factor) + 0.5f);
    p_color->g = (uint8_t)((p_color->g * factor) + 0.5f);
    p_color->b = (uint8_t)((p_color->b * factor) + 0.5f);
}

/* State machine output or action functions */

/* Other auxiliary functions */

/* Public functions -----------------------------------------------------------*/


/**
 * @brief Check if a new color has to be set.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_set_new_color (fsm_t *p_this) {
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    return p_fsm->new_color;

}

/**
 * @brief Check if the RGB light is set to be active (ON), independently if it is idle or not.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_active (fsm_t *p_this) {
    /*Casteamos el puntero genérico a nuestra estructura*/
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    /* 1. Return the flag status */
    return p_fsm->status;
}

/**
 * @brief Check if the RGB light is set to be inactive (OFF).
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_off (fsm_t *p_this) {
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    /* 1. Return the inverse flag status */
    return !(p_fsm-> status);
}

/**
 * @brief Turn the RGB light system ON for the first time.
 * 
 * @param p_this 
 */
static void do_set_on (fsm_t *p_this) {
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    port_rgb_light_set_rgb(p_fsm->rgb_light_id, color_off);
}

/**
 * @brief Set the color of the RGB LED according to the intensity measured by the ultrasound sensor.
 * 
 * @param p_this 
 */
static void do_set_color (fsm_t *p_this) {
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    /* 1. Correct the levels of the RGB LEDs according to the intensity. */
    /* Pasamos la DIRECCIÓN de nuestra copia (&current_color) y la intensidad actual de la FSM */
    _correct_rgb_light_levels(&p_fsm->color, p_fsm->intensity_perc);

    /* 2. Call function port_rgb_light_set_rgb() with the RGB LED ID and the color */
    port_rgb_light_set_rgb(p_fsm->rgb_light_id, p_fsm->color);

    /* 3. Reset the flag new_color to indicate that the color has been set */
    p_fsm->new_color = false;

    /* 4. Set the RGB light system to idle. */
    /* In this case, the RGB light system is active, but while the intensity is not changed, 
       the RGB light system is idle and can enter in a low power mode */
    p_fsm->idle = true;
}

/**
 * @brief Turn the RGB light system OFF.
 * 
 * @param p_this 
 */
static void do_set_off (fsm_t *p_this) {
    fsm_rgb_light_t *p_fsm = (fsm_rgb_light_t *)p_this;

    port_rgb_light_set_rgb(p_fsm->rgb_light_id, color_off);
    p_fsm->idle = false;
}


/* State machine input or transition functions */
fsm_trans_t fsm_trans_rgb_light[] = {
    /* Estado actual  |  Condición (Guarda)     |  Estado destino  |  Acción (Salida)  */
    { IDLE_RGB,          check_active,             SET_COLOR,         do_set_on },
    { SET_COLOR,         check_set_new_color,      SET_COLOR,         do_set_color },
    { SET_COLOR,         check_off,                IDLE_RGB,          do_set_off },
    { -1,                NULL,                     -1,                NULL }
};


/**
 * @brief Initialize an RGB light system FSM.
 * 
 * @param p_fsm_rgb_light 
 * @param rgb_light_id 
 */
static void fsm_rgb_light_init (fsm_rgb_light_t *p_fsm_rgb_light, uint8_t rgb_light_id) {
    /* 1. Call the fsm_init() to initialize the FSM. 
    Hacemos cast a fsm_t y pasamos la tabla de transiciones*/
    fsm_init((fsm_t *)p_fsm_rgb_light, fsm_trans_rgb_light);

    /* 2. Initialize the rgb_light_id (corrigiendo la errata del enunciado) */
    p_fsm_rgb_light->rgb_light_id = rgb_light_id;

    /* 3. Set the intensity_perc to MAX_LEVEL_INTENSITY. Initialize the color to OFF. */
    p_fsm_rgb_light->intensity_perc = MAX_LEVEL_INTENSITY; /* Revisa si en tu .h se llama intensity o intensity_perc */
    p_fsm_rgb_light->color = color_off;

    /* 4. Initialize the flags new_color, status, and idle to false. */
    p_fsm_rgb_light->new_color = false;
    p_fsm_rgb_light->status = false;
    p_fsm_rgb_light->idle = false;

    /* 5. Call function port_rgb_light_init() to initialize the HW. */
    port_rgb_light_init(rgb_light_id);
}

/**
 * @brief Create a new RGB light FSM.
 * 
 * @param rgb_light_id 
 * @return fsm_rgb_light_t* 
 */
fsm_rgb_light_t * fsm_rgb_light_new (uint8_t rgb_light_id) {
    /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_rgb_light_t *p_fsm_rgb_light = malloc(sizeof(fsm_rgb_light_t));
    fsm_rgb_light_init(p_fsm_rgb_light, rgb_light_id);
    return p_fsm_rgb_light;  
}

/**
 * @brief Destroy an RGB light FSM.
 * 
 * @param p_fsm 
 */
void fsm_rgb_light_destroy (fsm_rgb_light_t *p_fsm) {
    /* 1. Liberamos la memoria que fue reservada con malloc en la función _new() */
    if (p_fsm != NULL) {
        free(&p_fsm->f);
    }
}

/**
 * @brief Fire the RGB light FSM.
 * 
 * @param p_fsm 
 */
void fsm_rgb_light_fire (fsm_rgb_light_t *p_fsm) {
    fsm_fire((fsm_t *)p_fsm);
}

/**
 * @brief Set the color and intensity of the RGB light.
 * 
 * @param p_fsm 
 * @param color 
 * @param intensity_perc 
 */
void fsm_rgb_light_set_color_intensity (fsm_rgb_light_t *p_fsm, rgb_color_t color, uint8_t intensity_perc) {
    /* Protegemos el código por si nos pasan un puntero nulo */
    if (p_fsm != NULL) {
        /* 1. Set the color and intensity_perc fields of the RGB light system FSM. */
        p_fsm->color = color;
        p_fsm->intensity_perc = intensity_perc;

        /* 2. Set the new_color field accordingly to indicate that a new color has to be set. */
        p_fsm->new_color = true;

        /* Si le mandamos un color nuevo, el sistema ya no está en reposo */
        p_fsm->idle = false; 
    }
}

/**
 * @brief Get the status of the RGB light FSM.
 * 
 * @param p_fsm 
 * @return true 
 * @return false 
 */
bool fsm_rgb_light_get_status (fsm_rgb_light_t *p_fsm) {
    return p_fsm->status;
}

/**
 * @brief Set the status of the RGB light FSM.
 * 
 * @param p_fsm 
 * @param status 
 */
void fsm_rgb_light_set_status (fsm_rgb_light_t *p_fsm, bool status) {
    /* Protegemos el código por si nos pasan un puntero nulo */
    if (p_fsm != NULL) {
        /* 1. Update the field status with the received value */
        p_fsm->status = status;
    }
}

/**
 * @brief Check if the RGB light system is active.
 * 
 * @param p_fsm 
 * @return true 
 * @return false 
 */
bool fsm_rgb_light_check_activity (fsm_rgb_light_t *p_fsm) {
    /* 1. Return true if the RGB light system is active and it is not idle. Otherwise, return false. */
    return (p_fsm->status && !p_fsm->idle);
}