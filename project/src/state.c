#include "speedometer.h"
#include "state.h"
#include "start_menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "car_pixmaps.h"
#include "track_pixmap.h"
#include <math.h>
#include "kbc.h"
#include "pause_menu.h"

state_t* init_state() {
    state_t *state = malloc(sizeof(state_t));
    if (state == NULL) return NULL;

    state->tag = STATE_START;
    state->data.start.cursor_x = 400;
    state->data.start.cursor_y = 300;
    state->data.start.menu = NULL;
    return state;
}

void draw_state(state_t *state) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START:
            start_menu_draw(state->data.start.menu, state->data.start.cursor_x, state->data.start.cursor_y);
            break;
        case STATE_IN_GAME:
            //Clear the screen
            vg_buf_clear();
            // Always draw the game world first
            track_draw();
            draw_car(state->data.in_game.car);
            speedometer_draw(state->data.in_game.car->speed);
            if(state->data.in_game.pause){
                vg_buf_desaturate(); // Grey out the frozen game world
                pause_menu_draw(state->data.in_game.pause_menu, state->data.in_game.cursor_x, state->data.in_game.cursor_y);
                break;
            }
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void destroy_state(state_t *state) {
    if (state == NULL) return;
    if (state->tag == STATE_IN_GAME) {
        destroy_car(state->data.in_game.car);
        pause_menu_destroy(state->data.in_game.pause_menu);
    }
    free(state);
}

/* Encapsulates the full setup needed when transitioning to the in-game state */
void state_enter_in_game(state_t *state, font_t *font) {
    track_init((xpm_map_t) track_xpm);
    camera_init(1600, 1200);
    state->data.in_game.car = create_car(800, 1000, 0, 0, (xpm_map_t *) car_xpms);
    state->data.in_game.key_w = false;
    state->data.in_game.key_s = false;
    state->data.in_game.key_a = false;
    state->data.in_game.key_d = false;
    state->data.in_game.pause = false;
    state->data.in_game.cursor_x = 400;
    state->data.in_game.cursor_y = 300;
    state->data.in_game.pause_menu = pause_menu_create(font);
    speedometer_init();
}

/* Tears down all in-game resources and transitions back to the start menu */
void state_enter_start(state_t *state, font_t *font, int cursor_x, int cursor_y) {
    speedometer_destroy();
    destroy_car(state->data.in_game.car);
    state->data.in_game.car = NULL;
    pause_menu_destroy(state->data.in_game.pause_menu);
    state->data.in_game.pause_menu = NULL;
    track_free();

    state->tag = STATE_START;
    state->data.start.cursor_x = cursor_x;
    state->data.start.cursor_y = cursor_y;
    state->data.start.menu = start_menu_create(font);
}

void handle_mouse_event(state_t *state, struct packet *pp, int cursor_x, int cursor_y) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START:
            state->data.start.cursor_x = cursor_x;
            state->data.start.cursor_y = cursor_y;
            
            // Check for Left Click
            if (pp->lb) { 
                if (button_is_hovered(state->data.start.menu->start_btn, cursor_x, cursor_y)) {
                    font_t *font = state->data.start.menu->font; // save font BEFORE destroying menu
                    start_menu_destroy(state->data.start.menu);
                    state->data.start.menu = NULL;
                    state->tag = STATE_IN_GAME;
                    state_enter_in_game(state, font);
                } 
                else if (button_is_hovered(state->data.start.menu->exit_btn, cursor_x, cursor_y)) {
                    running = false;
                    start_menu_destroy(state->data.start.menu);
                }
            }
            break;
            
        case STATE_IN_GAME:
            state->data.in_game.cursor_x = cursor_x;
            state->data.in_game.cursor_y = cursor_y;
            if(pp->lb && state->data.in_game.pause && state->data.in_game.pause_menu != NULL) {
                if(button_is_hovered(state->data.in_game.pause_menu->resume_btn, cursor_x, cursor_y)) {
                    state->data.in_game.pause = false;
                }
                else if (button_is_hovered(state->data.in_game.pause_menu->exit_btn, cursor_x, cursor_y)) {
                    font_t *font = state->data.in_game.pause_menu->font;
                    state_enter_start(state, font, cursor_x, cursor_y);
                }
            }
            break;
            
        case STATE_GAME_OVER:
            break;
    }
}

void handle_kbd_event(state_t *state, uint8_t scancode) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START:
            break;
        case STATE_IN_GAME:
            // Allow pausing the game with ESC
            if (scancode == ESC_MAKE) {
                state->data.in_game.pause = !state->data.in_game.pause;
                break;
            }
            // Only handle WASD when not paused
            if(!state->data.in_game.pause) {
                // Track Make codes (press) and Break codes (release) for WASD
                if (scancode == W_MAKE) state->data.in_game.key_w = true;      // W Make
                else if (scancode == W_BREAK) state->data.in_game.key_w = false; // W Break

                if (scancode == S_MAKE) state->data.in_game.key_s = true;      // S Make
                else if (scancode == S_BREAK) state->data.in_game.key_s = false; // S Break

                if (scancode == A_MAKE) state->data.in_game.key_a = true;      // A Make
                else if (scancode == A_BREAK) state->data.in_game.key_a = false; // A Break

                if (scancode == D_MAKE) state->data.in_game.key_d = true;      // D Make
                else if (scancode == D_BREAK) state->data.in_game.key_d = false; // D Break
            }
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void update_state(state_t *state) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START:
            break;
        case STATE_IN_GAME: {
            if (state->data.in_game.pause) break; // Skip physics and camera while paused
            update_car_physics(state->data.in_game.car,
                               state->data.in_game.key_w,
                               state->data.in_game.key_s,
                               state->data.in_game.key_a,
                               state->data.in_game.key_d);
            /* Center camera on the car */
            car_t *c = state->data.in_game.car;
            int idx = (int)((c->angle + 11.25) / 22.5) % 16;
            camera_follow((int)c->x + c->sprites[idx]->width / 2,
                          (int)c->y + c->sprites[idx]->height / 2);
            break;
        }
        case STATE_GAME_OVER:
            break;
    }
}
