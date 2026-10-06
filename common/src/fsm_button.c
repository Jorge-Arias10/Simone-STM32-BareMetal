/**
 * @file fsm_button.c
 * @brief Button FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
/* HW dependent includes */
#include "port_button.h"
#include "port_system.h"

/* Project includes */
#include "fsm_button.h"
#include "fsm.h"

/* State machine input or transition functions */
//las de tipo check_...
/**
 * @brief check if the button has been pressed.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_button_pressed (fsm_t *p_this)
{
    fsm_button_t * p_fsm_button = (fsm_button_t *)(p_this); //transformacion de tipo de FSM general (fsm_t) a la FSM de tipo button (fsm_button_t)
    return port_button_get_pressed(p_fsm_button->button_id);
}

/**
 * @brief Check if the button has been released.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_button_released (fsm_t *p_this)
{
    fsm_button_t * p_fsm_button = (fsm_button_t *)(p_this); //transformacion de tipo de FSM general (fsm_t) a la FSM de tipo button (fsm_button_t)
    return !port_button_get_pressed(p_fsm_button->button_id);
}

/**
 * @brief Check if the debounce-time has passed.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_timeout (fsm_t *p_this)
{
    // Transformamos el puntero genérico al tipo de la FSM del botón
    fsm_button_t *p_fsm_button = (fsm_button_t *)(p_this);

    // 1. Llamar a port_system_get_millis() para obtener el tiempo actual
    uint32_t current_tick = port_system_get_millis();

    // 2 y 3. Comprobar si el tick actual es mayor que next_timeout
    if (current_tick > p_fsm_button->next_timeout) {
        return true;
    } else {
        return false;
}
}


/* State machine output or action functions */
// do_...

/**
 * @brief Store the system tick when the button was pressed.
 * 
 * @param p_this 
 */
static void do_store_tick_pressed(fsm_t *p_this) 
{
    fsm_button_t *p_fsm_button = (fsm_button_t *)(p_this);

    // 1. Obtener el tick actual del sistema
    uint32_t current_tick = port_system_get_millis();

    // 2. Guardar el tick actual en el campo tick_pressed
    p_fsm_button->tick_pressed = current_tick;

    // 3. Actualizar next_timeout (tick actual + tiempo de rebote)
    p_fsm_button->next_timeout = current_tick + p_fsm_button->debounce_time_ms;
}

/**
 * @brief Store the duration of the button press.
 * 
 * @param p_this 
 */
static void do_set_duration(fsm_t *p_this) 
{
    fsm_button_t *p_fsm_button = (fsm_button_t *)(p_this);

    // 1. Obtener el tick actual del sistema
    uint32_t current_tick = port_system_get_millis();

    // 2. Calcular la duración: tick actual menos cuando se pulsó
    p_fsm_button->duration = current_tick - p_fsm_button->tick_pressed;

    // 3. Actualizar next_timeout sumando el tiempo de rebote al tick actual
    p_fsm_button->next_timeout = current_tick + p_fsm_button->debounce_time_ms;
}

/**
 * @brief Array representing the transitions table of the FSM button.
 * 
 */
static fsm_trans_t fsm_trans_button[] = {
    // { EstadoInicial, FuncCompruebaCondicion, EstadoSig, FuncAccionesSiTransicion }
    
    // De RELEASED a PRESSED_WAIT cuando se pulsa el botón
    { BUTTON_RELEASED, check_button_pressed, BUTTON_PRESSED_WAIT, do_store_tick_pressed },
    
    // De PRESSED_WAIT a PRESSED cuando pasa el tiempo de rebote
    { BUTTON_PRESSED_WAIT, check_timeout, BUTTON_PRESSED, NULL },
    
    // De PRESSED a RELEASED_WAIT cuando se suelta el botón
    { BUTTON_PRESSED, check_button_released, BUTTON_RELEASED_WAIT, do_set_duration },
    
    // De RELEASED_WAIT a RELEASED cuando pasa el tiempo de rebote
    { BUTTON_RELEASED_WAIT, check_timeout, BUTTON_RELEASED, NULL },
    
    // Fila de fin de tabla obligatoria para la librería fsm.c
    { -1, NULL, -1, NULL }
};


/* Other auxiliary functions */
/**
 * @brief Initialize a button FSM. This function initializes the default values of the FSM struct and calls to the port to initialize the associated HW given the ID.
 * 
 * @param p_fsm_button 
 * @param debounce_time 
 * @param button_id 
 */
void fsm_button_init(fsm_button_t *p_fsm_button, uint32_t debounce_time, uint8_t button_id)
{
    // 1. Inicializar la FSM genérica con la tabla de transiciones
    fsm_init(&p_fsm_button->f, fsm_trans_button);

    /* TODO alumnos: */
    // 2. Asignar los valores de configuración recibidos
    p_fsm_button->debounce_time_ms = debounce_time;
    p_fsm_button->button_id = button_id;

    // 3. Inicializar los contadores de tiempo y duración a 0
    p_fsm_button->tick_pressed = 0;
    p_fsm_button->duration = 0;

    // 4. Llamar al port para inicializar el hardware del botón
    port_button_init(p_fsm_button->button_id);

}

/* Public functions -----------------------------------------------------------*/
fsm_button_t *fsm_button_new(uint32_t debounce_time, uint8_t button_id)
{
    fsm_button_t *p_fsm_button = malloc(sizeof(fsm_button_t)); /* Do malloc to reserve memory of all other FSM elements, although it is interpreted as fsm_t (the first element of the structure) */
    fsm_button_init(p_fsm_button, debounce_time, button_id);   /* Initialize the FSM */
    return p_fsm_button;                                       /* Composite pattern: return the fsm_t pointer as a fsm_button_t pointer */
}

/* FSM-interface functions. These functions are used to interact with the FSM */
/**
 * @brief This function is used to fire the button FSM. It is used to check the transitions and execute the actions of the button FSM.
 * 
 * @param p_fsm 
 */
void fsm_button_fire(fsm_button_t *p_fsm)
{
    fsm_fire(&p_fsm->f); // Is it also possible to it in this way: fsm_fire((fsm_t *)p_fsm);
}

/**
 * @brief This function destroys the button FSM and frees the memory.
 * 
 * @param p_fsm 
 */
void fsm_button_destroy(fsm_button_t *p_fsm)
{
    free(&p_fsm->f);
}

/**
 * @brief Return the duration of the last button press.
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_duration(fsm_button_t *p_fsm) 
{
    // 1. Recuperar y devolver el campo duration
    return p_fsm->duration;
}

/**
 * @brief Reset the duration of the last button press.
 * 
 * @param p_fsm 
 */
void fsm_button_reset_duration(fsm_button_t * p_fsm) 
{
    // TODO alumnos:
    // 1. Set to 0 the field duration
    p_fsm->duration = 0;
}

/**
 * @brief Get the debounce time of the button FSM. This function returns the debounce time of the button FSM.
 * 
 * @param p_fsm 
 * @return uint32_t 
 */
uint32_t fsm_button_get_debounce_time_ms(fsm_button_t *p_fsm) 
{
    // 1. Recuperar y devolver el campo debounce_time_ms
    return p_fsm->debounce_time_ms;
}

/** V4  */
/**
 * @brief Check if the button FSM is active, or not.
 * 
 * @param p_fsm 
 * @return bool
 */
bool fsm_button_check_activity(fsm_button_t * p_fsm) {
    /* 1 y 2. Get the field current_state and return false if it is BUTTON_RELEASED */
    return (p_fsm->f.current_state != BUTTON_RELEASED);
}