#ifndef _STATE_H_
#define _STATE_H_

#include <lcom/lcf.h>
#include <stdbool.h>
#include "car.h"
#include "input.h"
#include "start_menu.h"
#include "camera.h"
#include "track.h"
#include "pause_menu.h"
#include "font.h"
#include "race.h"
#include "rtc.h"
#include "cursor.h"

extern bool running;

typedef enum
{
    STATE_START,
    STATE_LOADING,
    STATE_IN_GAME,
    STATE_GAME_OVER
} state_tag_t;

typedef struct
{
    state_tag_t tag;
    cursor_t *cursor;
    union {
        struct {
            start_menu_t *menu;
        } start;

        struct
        {
            font_t *font;
            int track_index;
            int car_index;
            bool drawn;
        } loading;

        struct {
            car_t *car;
            track_t *track;
            camera_t *camera;
            font_t *font;
            race_t race;
            game_input_t input;
            bool pause;
            pause_menu_t *pause_menu;
        } in_game;

        struct
        {
            unsigned final_time;
            rtc_date date;
        } game_over;
    } data;
} state_t;

/**
    @brief Initializes the game state to the starting screen.
    @return Pointer to the initialized game state.
**/
state_t *init_state();

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
 * @brief Transitions the game to the loading state before starting the race.
 *
 * @param state Pointer to the current game state.
 * @param font The font to use for rendering the loading screen.
 */
void state_enter_loading(state_t *state, font_t *font);

/**
 * @brief Transitions the game back to the start-menu state, freeing in-game resources.
 *
 * @param state Pointer to the current game state.
 * @param font  The font to reuse for the start menu.
 */
void state_enter_start(state_t *state, font_t *font);

/**
 * @brief Transitions the game to the game-over state.
 *
 * @param state Pointer to the current game state.
 * @param font  The font to use for game over menu.
 * @param final_time The final race time in seconds.
 */
void state_enter_game_over(state_t *state, font_t *font, unsigned final_time);

#endif /* _STATE_H_ */
