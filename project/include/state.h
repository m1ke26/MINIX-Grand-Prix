#ifndef _STATE_H_
#define _STATE_H_

#include <lcom/lcf.h>
#include <stdbool.h>
#include "car.h"
#include "start_menu.h"

extern bool running;

typedef enum {
    STATE_START,
    STATE_IN_GAME,
    STATE_GAME_OVER
} state_tag_t;

typedef struct {
    state_tag_t tag;
    union {
        struct {
            int cursor_x;
            int cursor_y;
            start_menu_t *menu;
        } start;

        struct {
            car_t car;
            unsigned laps;
            unsigned seconds_elapsed;
            bool key_w;
            bool key_s;
            bool key_a;
            bool key_d;
        } in_game;

        struct {
            unsigned final_time;
            unsigned laps_done;
        } game_over;
    } data;
} state_t;

/**
    @brief Initializes the game state to the starting screen.
    @return Pointer to the initialized game state.
**/
state_t* init_state();

/**
    @brief Draws the current state on the screen, including all relevant elements based on the state tag.
    @param state Pointer to the current game state.
**/
void draw_state(state_t *state);

/**
    @brief Destroys the game state and frees any allocated resources.
    @param state Pointer to the current game state.
**/
void destroy_state(state_t *state);

/**
    @brief Updates the game state based on user input and game events. This function should be called in the main game loop.    
    @param state Pointer to the current game state.
**/
void update_state(state_t *state);

/**
    @brief Handles mouse events depending on the current state.
    @param state Pointer to the current game state.
    @param pp Pointer to the packet containing the mouse event.
    @param cursor_x The x-coordinate of the mouse cursor.
    @param cursor_y The y-coordinate of the mouse cursor.
**/  
void handle_mouse_event(state_t *state, struct packet *pp, int cursor_x, int cursor_y);

/**
    @brief Handles keyboard events depending on the current state.
    @param state Pointer to the current game state.
    @param scancode The scancode of the key pressed.
**/
void handle_kbd_event(state_t *state, uint8_t scancode);

/**
 * @brief Transitions the game to the in-game state.
 * 
 * @param state Pointer to the current game state.
 */
void state_enter_in_game(state_t *state);

#endif /* _STATE_H_ */
