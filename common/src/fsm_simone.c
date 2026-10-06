/**
 * @file fsm_simone.c
 * @brief Simone FSM main file.
 * @author alumno1
 * @author alumno2
 * @date fecha
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* HW dependent includes */
#include "port_simone.h"

/* Project includes */
#include "fsm_simone.h"       
#include "port_system.h"      
#include "rgb_colors.h"

const rgb_color_t *p_colors_library[] = {&color_red, &color_green, &color_blue, &color_yellow, &color_turquoise, &color_white};

/* Private functions -----------------------------------------------------------*/

static char _get_key_from_color(rgb_color_t color)
{
    if (color.r == color_red.r && color.g == color_red.g && color.b == color_red.b) {
        return KEY_RED;
    } else if (color.r == color_green.r && color.g == color_green.g && color.b == color_green.b) {
        return KEY_GREEN;
    } else if (color.r == color_blue.r && color.g == color_blue.g && color.b == color_blue.b) {
        return KEY_BLUE;
    } else if (color.r == color_yellow.r && color.g == color_yellow.g && color.b == color_yellow.b) {
        return KEY_YELLOW;
    } else if (color.r == color_turquoise.r && color.g == color_turquoise.g && color.b == color_turquoise.b) {
        return KEY_TURQUOISE;
    } else if (color.r == color_white.r && color.g == color_white.g && color.b == color_white.b) {
        return KEY_WHITE;
    } else {
        return KEY_INVALID_COLOR;
    }
}

static rgb_color_t _get_color_from_key(char key)
{
    switch (key) {
        case KEY_RED: return color_red;
        case KEY_GREEN: return color_green;
        case KEY_BLUE: return color_blue;
        case KEY_YELLOW: return color_yellow;
        case KEY_TURQUOISE: return color_turquoise;
        case KEY_WHITE: return color_white;
        default: return color_off;
    }
}

static void _add_color(fsm_simone_t *p_fsm_simone)
{
    uint8_t min_intensity;
    uint8_t max_intensity = LEVEL_MAX_INTENSITY;

    // 1. Determinar el mínimo según el nivel actual
    if (p_fsm_simone->level == LEVEL_EASY) min_intensity = LEVEL_EASY_MIN_INTENSITY;
    else if (p_fsm_simone->level == LEVEL_MEDIUM) min_intensity = LEVEL_MEDIUM_MIN_INTENSITY;
    else min_intensity = LEVEL_HARD_MIN_INTENSITY;

    // 2. Generar color e intensidad aleatorios
    uint8_t color_idx = rand() % NUMBER_OF_COLORS_GAME;
    uint8_t random_intensity = (rand() % (max_intensity - min_intensity + 1)) + min_intensity;

    // 3. Gestionar el límite de la secuencia
    if (p_fsm_simone->seq_idx >= SEQUENCE_LENGTH) {
        p_fsm_simone->seq_idx = 0; // Reiniciamos si llegamos al tope
    }

    // 4. Guardar en los arrays paralelos
    p_fsm_simone->seq_colors[p_fsm_simone->seq_idx] = *p_colors_library[color_idx];
    p_fsm_simone->seq_intensities[p_fsm_simone->seq_idx] = random_intensity;

    // 5. Incrementar el índice de la secuencia
    p_fsm_simone->seq_idx++;
}

/* ========================================================================== */
/* STATE MACHINE INPUT OR TRANSITION FUNCTIONS (GUARDS)                          */
/* ========================================================================== */

/** IDLE Y GENERAL */
/**
 * @brief Comprobar si el botón lleva pulsado el tiempo SIMONE_ON_OFF_PRESS_TIME_MS (en main)
 * 
 * @param p_this 
 * @return true 
 * @return false
 */
static bool check_on(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    return (fsm_button_check_activity(p_fsm->p_fsm_button) && 
            fsm_button_get_duration(p_fsm->p_fsm_button) >= p_fsm->on_off_press_time_ms);
}

/**
 * @brief Comprueba si algún elemento del sistema está activo.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_activity(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    return (fsm_button_check_activity(p_fsm->p_fsm_button) ||
            fsm_keyboard_check_activity(p_fsm->p_fsm_keyboard) ||
            fsm_rgb_light_check_activity(p_fsm->p_fsm_rgb_light));
}

/**
 * @brief Comprueba si todos los elementos del sistema están inactivos.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_no_activity(fsm_t *p_this) {
    return !check_activity(p_this);
}

/** ADD_COLOR */

/**
 * @brief Comprueba si un color se ha añadido a la secuqencia.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_color_added(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    /* Si el índice de la secuencia es distinto al del jugador, es que hay colores nuevos */
    return (p_fsm->seq_idx != p_fsm->player_idx);
}

/**
 * @brief función auxiliar para saber cuánto tiempo encender el LED según el nivel
 * 
 * @param level 
 * @return uint32_t 
 */
static uint32_t _get_time_on(uint8_t level) {
    if (level == LEVEL_HARD) return SIMONE_TIME_ON_LEVEL_HARD_MS;
    if (level == LEVEL_MEDIUM) return SIMONE_TIME_ON_LEVEL_MEDIUM_MS;
    return SIMONE_TIME_ON_LEVEL_EASY_MS;
}

/** PLAYBACK Y SLEEP_WHILE_PLAYBACk */

/**
 * @brief Comprueba si el temporizador de Simone ha expirado.
 * @param p_this Puntero a la FSM genérica.
 * @return true si el tiempo ha terminado, false en caso contrario.
 */
static bool check_playback_color_timeout(fsm_t *p_this) {
    /* Simplemente le preguntamos al PORT si el flag de timeout está a true */
    return port_simone_get_timeout_status();
}

/**
 * @brief Comprueba si el temporizador de Simone aún NO ha expirado.
 * @param p_this Puntero a la FSM genérica.
 * @return true si todavía queda tiempo, false si ya terminó.
 */
static bool check_no_timeout(fsm_t *p_this) {
    /* Es la inversa de la anterior */
    return !port_simone_get_timeout_status();
}

/**
 * @brief Comprueba si el botón se ha pulesado el suficiente tiempo para encender el sistema IDLE.
 * @param p_this Puntero a la FSM genérica.
 */
static bool check_off(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    /* misma lógica que check_on */
    return (fsm_button_check_activity(p_fsm->p_fsm_button) && 
            fsm_button_get_duration(p_fsm->p_fsm_button) >= p_fsm->on_off_press_time_ms);
}

/**
 * @brief Comprobar si el playback de la secuencia ha terminado para que el jugador empiece una nueva secuencia.
 * 
 * @param p_this 
 * @return true 
 * @return false 
 */
static bool check_playback_over(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    /* 1. índice ha llegado al marcador de fin (0xFF)? 
       2. ¿Ha terminado el tiempo de la última pausa? */
    return (p_fsm->playback_idx >= p_fsm->seq_idx && port_simone_get_timeout_status());
}

/** WAIT_KEY */

/**
 * @brief (2) Comprueba si el jugador ha ganado el juego por completo.
 */
static bool check_winner(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    /* 1. ha terminado la secuencia? 2. ha llegado a long max? 3. ¿Está en nivel difícil? */
    return (p_fsm->player_idx >= p_fsm->seq_idx) && 
           (p_fsm->seq_idx == SEQUENCE_LENGTH) && 
           (p_fsm->level == LEVEL_HARD);
}

/**
 * @brief (3) Comprueba si se ha agotado el tiempo para pulsar.
 */
static bool check_player_key_timeout(fsm_t *p_this) {
    return port_simone_get_timeout_status();
}

/**
 * @brief (4) Comprueba si el jugador ha acertado todos los colores de ESTA ronda.
 */
static bool check_player_round_end(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    /* ¿Acertó los colores de la ronda actual PERO todavía no ha ganado el juego entero? */
    return (p_fsm->player_idx >= p_fsm->seq_idx) && !check_winner(p_this);
}

/**
 * @brief (5) Comprueba si el usuario ha pulsado alguna tecla.
 */
static bool check_any_key_pressed(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    /* Usamos la función de la librería del teclado que ya sabe qué es válido y qué no */
    return fsm_keyboard_get_is_valid_key(p_fsm->p_fsm_keyboard);
}

/* VERIFY_INPUT */

/**
 * @brief (1) Comprueba si el tiempo de feedback pasó y la tecla ES CORRECTA.
 */
static bool check_input_valid(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    /* 1. Espera visual: Si el timer NO ha terminado, devolvemos false (nos quedamos aquí) */
    if (!port_simone_get_timeout_status()) {
        return false;
    }
    
    /* 2. Validación lógica: Traducimos el color esperado a tecla y comparamos */
    char expected_key = _get_key_from_color(p_fsm->seq_colors[p_fsm->player_idx]);
    
    return (p_fsm->player_key == expected_key);
}

/**
 * @brief (2) Comprueba si el tiempo de feedback pasó y la tecla ES INCORRECTA.
 */
static bool check_input_invalid(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    /* 1. Espera visual: Si el timer NO ha terminado, devolvemos false */
    if (!port_simone_get_timeout_status()) {
        return false;
    }
    
    /* 2. Validación lógica: Comparar si la tecla es DISTINTA a la esperada */
    char expected_key = _get_key_from_color(p_fsm->seq_colors[p_fsm->player_idx]);
    
    return (p_fsm->player_key != expected_key);
}

/* ========================================================================== */
/* STATE MACHINE OUTPUT OR ACTION FUNCTIONS                                   */
/* ========================================================================== */

/* IDLE Y GENERAL */

/**
 * @brief Inicializa el juego.
 * 
 * @param p_this 
 */
static void do_init_game(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;

    // Reset de periféricos
    fsm_button_reset_duration(p_fsm->p_fsm_button);
    fsm_keyboard_reset_key_value(p_fsm->p_fsm_keyboard);

    // Reset de índices y flags
    p_fsm->seq_idx = 0;
    p_fsm->playback_idx = 0;
    p_fsm->player_idx = 0;
    p_fsm->playback_over = false;
    p_fsm->player_key = KEY_INVALID_COLOR;
    p_fsm->level = LEVEL_EASY;

    // Limpiar arrays (color_off e intensidad 0)
    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
        p_fsm->seq_colors[i] = color_off;
        p_fsm->seq_intensities[i] = 0;
    }

    // Añadir el primer color de la ronda 1
    _add_color(p_fsm);

    // Activar el status de la FSM de luz para que pueda brillar
    fsm_rgb_light_set_status(p_fsm->p_fsm_rgb_light, true);

    printf("[SIMONE][%ld] Simone game INIT\n", port_system_get_millis());
}

/**
 * @brief Empieza el modo bajo consumo mientras Simone es IDLE.
 * 
 * @param p_this 
 */
static void do_sleep_idle(fsm_t *p_this) {
    port_system_sleep();
}

/** PLAYBACK Y SLEEP_WHILE_PLAYBACK */
/** FLUJOGRAMA */
static void do_playback(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;

    /* ¿sequence completed? Si el marcador de fin (0xFF) está puesto, salimos inmediatamente */
    if (p_fsm->playback_idx >= 0xFF) {
        return; 
    }

    // 1. Reset flag timer y Stop scan keyboard (Protección contra entradas espurias)
    port_simone_set_timeout_status(false);
    fsm_keyboard_stop_scan(p_fsm->p_fsm_keyboard);

    // 2. ¿playback_over es false? -> FASE ENCENDIDO
    if (!p_fsm->playback_over) {
        // Recuperar color e intensidad del array usando playback_idx
        rgb_color_t color = p_fsm->seq_colors[p_fsm->playback_idx];
        uint8_t intensity = p_fsm->seq_intensities[p_fsm->playback_idx];

        // Encender LED
        fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color, intensity);

        // Programar timer con duración ON según nivel
        port_simone_set_timer_timeout(_get_time_on(p_fsm->level));

        // Cambiar fase a true para que la próxima vez pase por el otro lado
        p_fsm->playback_over = true;
    } 
    // 3. ¿playback_over es true? -> FASE PAUSA (APAGADO)
    else {
        // Set LED RGB color OFF
        fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color_off, 0);

        // Programar timer con duración fija de pausa
        port_simone_set_timer_timeout(SIMONE_TIME_OFF_BETWEEN_COLORS_MS);

        // Lógica de índices
        p_fsm->playback_idx++;
        p_fsm->playback_over = false; // Reset para el siguiente color

        // ¿Hemos llegado al final de la secuencia?
        if (p_fsm->playback_idx >= p_fsm->seq_idx) {
            // "Indicate end of playback" -> Usamos un marcador (ej: valor 0xFF)
            p_fsm->playback_idx = 0xFF; 
        }
    }
}

/**
 * @brief Pone el sistema en modo bajo consumo mientras se reproduce la secuencia.
 * @param p_this Puntero a la FSM genérica.
 */
static void do_sleep_playback(fsm_t *p_this) {

    /* 1. Llamamos a la función de bajo consumo del PORT del sistema.
       Esta función detiene el SysTick y ejecuta la instrucción WFI (Wait For Interrupt). */
    port_system_sleep();
}

/**
 * @brief Activa el IDLE del sistema sImone.
 * 
 * @param p_this Puntero a la FSM genérica.
 */
static void do_stop_simone(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    fsm_button_reset_duration(p_fsm->p_fsm_button);
    fsm_rgb_light_set_status(p_fsm->p_fsm_rgb_light, false); // Desactivar LED
    p_fsm->level = LEVEL_EASY;
    
    printf("[SIMONE][%ld] Game OVER. Press button to start a new game.\n", port_system_get_millis());
}

/**
 * @brief Empezar la secuencia de input del jugador y preparar el sistema para su input.
 * 
 * @param p_this 
 */
static void do_start_player_sequence(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;

    p_fsm->playback_over = false;
    p_fsm->player_idx = 0; // El jugador empieza desde el primer color
    
    /* Aseguramos que el LED esté apagado */
    fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color_off, 0);
    
    /* Programamos el tiempo máximo de espera para la primera tecla */
    port_simone_set_timer_timeout(SIMONE_TIME_WAIT_INPUT_MS);
    port_simone_set_timeout_status(false); // Limpiamos el flag para empezar la cuenta limpia
    
    /* Activamos el teclado, para poder leer las teclas */
    fsm_keyboard_start_scan(p_fsm->p_fsm_keyboard);
    
    printf("[SIMONE][%ld] Your turn! You have %d seconds to press each key.\n", 
           port_system_get_millis(), SIMONE_TIME_WAIT_INPUT_MS / 1000);
}

/** WAIT_KEY */
/**
 * @brief (2) Acción de victoria.
 */
static void do_winner(fsm_t *p_this) {
    port_simone_stop_timer();
    printf("[SIMONE][%ld] YOU WIN!! You are a Simone Master!\n", port_system_get_millis());
}

/**
 * @brief (3) Acción de derrota por tiempo agotado.
 */
static void do_game_over_timeout(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    port_simone_stop_timer();
    fsm_keyboard_stop_scan(p_fsm->p_fsm_keyboard);
    
    // Reseteo de índices por seguridad
    p_fsm->seq_idx = 0;
    p_fsm->player_idx = 0;
    p_fsm->playback_idx = 0;
    
    printf("[SIMONE][%ld] GAME OVER by Timeout! You survived until level %d, sequence length %d.\n", 
           port_system_get_millis(), p_fsm->level, p_fsm->seq_idx);
}

/**
 * @brief (4) Acción de fin de ronda.
 */
static void do_add_color(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;

    /* 1. Reset player_idx, playback_idx and playback_over */
    p_fsm->player_idx = 0;
    p_fsm->playback_idx = 0;
    p_fsm->playback_over = false;

    /* 2. Rombo del flujograma: ¿Array full AND level < HARD? */
    if (p_fsm->seq_idx >= SEQUENCE_LENGTH && p_fsm->level < LEVEL_HARD) {
        p_fsm->level++;       
        p_fsm->seq_idx = 0;   
        printf("[SIMONE][%ld] LEVEL UP! Moving to level %d\n", port_system_get_millis(), p_fsm->level);
    }

    /* 3. esta mal en la api, _add_sequence(...) NO, _add_color */
    _add_color(p_fsm); 
}

/**
 * @brief (5) Capturar la entrada del teclado y dar feedback visual.
 */
static void do_capture_input(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;

    /* Obtener y guardar la tecla */
    p_fsm->player_key = fsm_keyboard_get_key_value(p_fsm->p_fsm_keyboard);
    
    /* Resetear el valor de la tecla en el HW para no leerla 2 veces */
    fsm_keyboard_reset_key_value(p_fsm->p_fsm_keyboard);

    /* Traducir tecla a color y encender LED a máxima potencia */
    rgb_color_t color = _get_color_from_key(p_fsm->player_key);
    fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color, LEVEL_MAX_INTENSITY);

    /* Poner temporizador para el feedback visual */
    port_simone_set_timer_timeout(SIMONE_TIME_VISUAL_FEEDBACK_MS);
    port_simone_set_timeout_status(false);
}

/** VERIFY_INPUT */
/**
 * @brief (1) Acción al acertar una tecla.
 */
static void do_valid_key(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    /* Apagamos el LED de feedback */
    fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color_off, 0);
    
    /* Aumentamos el índice del jugador */
    p_fsm->player_idx++;
    
    /* Limpiamos la tecla guardada por seguridad */
    p_fsm->player_key = KEY_INVALID_COLOR;
    
    /* Reiniciamos timer para darle tiempo a pulsar la SIGUIENTE tecla */
    port_simone_set_timer_timeout(SIMONE_TIME_WAIT_INPUT_MS);
    port_simone_set_timeout_status(false);
}

/**
 * @brief (2) Acción al fallar una tecla (Game Over).
 */
static void do_game_over_invalid_key(fsm_t *p_this) {
    fsm_simone_t *p_fsm = (fsm_simone_t *)p_this;
    
    /* Apagamos el LED */
    fsm_rgb_light_set_color_intensity(p_fsm->p_fsm_rgb_light, color_off, 0);
    
    /* obtenemos qqué se esperaba para imprimirlo en el mensaje */
    char expected_key = _get_key_from_color(p_fsm->seq_colors[p_fsm->player_idx]);
    
    printf("[SIMONE][%ld] GAME OVER! Wrong key. Expected '%c' but got '%c'.\n", 
           port_system_get_millis(), expected_key, p_fsm->player_key);
    
    /* Reseteo completo del juego */
    p_fsm->seq_idx = 0;
    p_fsm->player_idx = 0;
    p_fsm->playback_idx = 0;
    p_fsm->player_key = KEY_INVALID_COLOR;
    p_fsm->level = LEVEL_EASY;
    
    /* Detenemos el hardware de la FSM de Simone */
    port_simone_stop_timer();
    fsm_keyboard_stop_scan(p_fsm->p_fsm_keyboard);
}

/* ========================================================================== */
/* TABLA DE TRANSICIONES DE FSM SIMONE                                        */
/* ========================================================================== */

/**
 * @brief Tabla de transiciones de V4 de FSM SIMONE.
 * 
 */
static fsm_trans_t fsm_trans_simone[] = {
    /* Estado Origen         | Condición (Guarda)              | Estado Destino         | Acción (Salida) */
    
    /** --- IDLE --- */
    /* Encendido por botón */
    { IDLE,                    check_on,                         ADD_COLOR,               do_init_game },
    /* Inactividad -> Bajo consumo */
    { IDLE,                    check_no_activity,                SLEEP_WHILE_IDLE,        do_sleep_idle },

    /** --- ADD_COLOR --- */
    /* Automático tras añadir color a la secuencia */
    { ADD_COLOR,               check_color_added,                PLAYBACK,                do_playback },

    /** --- PLAYBACK --- */
    /* Apagado manual (Prioridad alta) */
    { PLAYBACK,                check_off,                        IDLE,                    do_stop_simone },
    /* Fin de reproducción -> Turno del jugador */
    { PLAYBACK,                check_playback_over,              WAIT_KEY,                do_start_player_sequence },
    /* Reproduciendo -> Dormir mientras corre el timer */
    { PLAYBACK,                check_no_timeout,                 SLEEP_WHILE_PLAYBACK,    do_sleep_playback },

    /** --- SLEEP_WHILE_PLAYBACK --- */
    /* Fin del timer de color/pausa -> Cambiar estado LED */
    { SLEEP_WHILE_PLAYBACK,    check_playback_color_timeout,     PLAYBACK,                do_playback },
    /* Autotransición por inactividad (útil en depuración) */
    { SLEEP_WHILE_PLAYBACK,    check_no_activity,                SLEEP_WHILE_PLAYBACK,    do_sleep_playback },

    /** --- WAIT_KEY (Turno del jugador) --- */
    /* Apagado manual (Prioridad alta) */
    { WAIT_KEY,                check_off,                        IDLE,                    do_stop_simone },
    /* Victoria final (Juego completado) */
    { WAIT_KEY,                check_winner,                     IDLE,                    do_winner },
    /* Fin de ronda (Secuencia completada, siguiente nivel/ronda) */
    { WAIT_KEY,                check_player_round_end,           ADD_COLOR,               do_add_color },
    /* Tecla pulsada -> Verificar */
    { WAIT_KEY,                check_any_key_pressed,            VERIFY_INPUT,            do_capture_input },
    /* Derrota por tiempo de inactividad */
    { WAIT_KEY,                check_player_key_timeout,         IDLE,                    do_game_over_timeout },

    /** --- VERIFY_INPUT --- */
    /* Tecla correcta (tras el tiempo de feedback visual) */
    { VERIFY_INPUT,            check_input_valid,                WAIT_KEY,                do_valid_key },
    /* Tecla incorrecta -> Derrota */
    { VERIFY_INPUT,            check_input_invalid,              IDLE,                    do_game_over_invalid_key },

    /** --- SLEEP_WHILE_IDLE --- */
    /* Detectada actividad -> Despertar */
    { SLEEP_WHILE_IDLE,        check_activity,                   IDLE,                    NULL },
    /* Autotransición por inactividad (útil en depuración) */
    { SLEEP_WHILE_IDLE,        check_no_activity,                SLEEP_WHILE_IDLE,        do_sleep_idle },

    /* Marcador obligatorio de final de tabla */
    { -1, NULL, -1, NULL }
};

/* ========================================================================== */
/* PUBLIC FUNCTIONS                                                         */
/* ========================================================================== */

/**
 * @brief Crea una nueva fsm Simone.
 * 
 * @param p_fsm_simone 
 * @param p_fsm_button 
 * @param on_off_press_time_ms 
 * @param p_fsm_keyboard 
 * @param p_fsm_rgb_light 
 * @param level 
 */
static void fsm_simone_init(fsm_simone_t *p_fsm_simone, fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    /* 1. Inicializar la FSM genérica con la tabla de transiciones */
    fsm_init((fsm_t *)p_fsm_simone, fsm_trans_simone);

    /* 2. Inicializar el HW asociado a la FSM de Simone */
    port_simone_init();

    /* 3. Guardar los punteros y parámetros recibidos */
    p_fsm_simone->p_fsm_button = p_fsm_button;
    p_fsm_simone->p_fsm_keyboard = p_fsm_keyboard;
    p_fsm_simone->p_fsm_rgb_light = p_fsm_rgb_light;
    p_fsm_simone->on_off_press_time_ms = on_off_press_time_ms;
    
    /* Variables de control a estado inicial */
    p_fsm_simone->level = LEVEL_EASY;
    p_fsm_simone->seq_idx = 0;
    p_fsm_simone->playback_idx = 0;
    p_fsm_simone->player_idx = 0;
    p_fsm_simone->playback_over = false;
    p_fsm_simone->player_key = KEY_INVALID_COLOR;

    /* 4. Inicializar semilla aleatoria */
    srand(time(NULL)); 

    printf("[SIMONE] System initialized. Press the user button to start a new game!\n");
}

/**
 * @brief Crea una nueva fsm Simone.
 * 
 * @param p_fsm_button 
 * @param on_off_press_time_ms 
 * @param p_fsm_keyboard 
 * @param p_fsm_rgb_light 
 * @param level 
 * @return fsm_simone_t* 
 */
fsm_simone_t *fsm_simone_new(fsm_button_t *p_fsm_button, uint32_t on_off_press_time_ms, fsm_keyboard_t *p_fsm_keyboard, fsm_rgb_light_t *p_fsm_rgb_light, uint8_t level)
{
    fsm_simone_t *p_fsm = (fsm_simone_t *)malloc(sizeof(fsm_simone_t));
    if (p_fsm != NULL) {
        fsm_simone_init(p_fsm, p_fsm_button, on_off_press_time_ms, p_fsm_keyboard, p_fsm_rgb_light, level);
    }
    return p_fsm;
}

/**
 * @brief Fire la SIMONE FSM.
 * @param p_fsm_simone Puntero a la FSM de Simone.
 */
void fsm_simone_fire(fsm_simone_t *p_fsm_simone)
{
    if (p_fsm_simone != NULL) {
        fsm_fire((fsm_t *)p_fsm_simone);
    }
}

/**
 * @brief Destruye la máquina de estados y libera su memoria.
 * @param p_fsm_simone Puntero a la FSM de Simone.
 */
void fsm_simone_destroy(fsm_simone_t *p_fsm_simone)
{
    if (p_fsm_simone != NULL) {
        free(p_fsm_simone);
    }
}