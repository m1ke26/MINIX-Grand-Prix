#include "state.h"
#include "start_menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "pixmaps.h"
#include "track_pixmap.h"
#include <math.h>
#include "kbc.h"

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
            track_draw();
            draw_car(&state->data.in_game.car);
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void destroy_state(state_t *state) {
    if (state == NULL) return;
    free(state);
}

/* Encapsulates the full setup needed when transitioning to the in-game state */
void state_enter_in_game(state_t *state) {
    track_init((xpm_map_t) track_xpm);
    camera_init(1600, 1200);
    car_t *new_car = create_car(800, 1000, 0, 0, (xpm_map_t *) car_xpms);
    state->data.in_game.car = *new_car;
    free(new_car);
    state->data.in_game.key_w = false;
    state->data.in_game.key_s = false;
    state->data.in_game.key_a = false;
    state->data.in_game.key_d = false;
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
                    start_menu_destroy(state->data.start.menu);
                    state->tag = STATE_IN_GAME;
                    state_enter_in_game(state);
                } 
                else if (button_is_hovered(state->data.start.menu->exit_btn, cursor_x, cursor_y)) {
                    running = false;
                }
            }
            break;
            
        case STATE_IN_GAME:
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
            // Track Make codes (press) and Break codes (release) for WASD
            if (scancode == W_MAKE) state->data.in_game.key_w = true;      // W Make
            else if (scancode == W_BREAK) state->data.in_game.key_w = false; // W Break

            if (scancode == S_MAKE) state->data.in_game.key_s = true;      // S Make
            else if (scancode == S_BREAK) state->data.in_game.key_s = false; // S Break

            if (scancode == A_MAKE) state->data.in_game.key_a = true;      // A Make
            else if (scancode == A_BREAK) state->data.in_game.key_a = false; // A Break

            if (scancode == D_MAKE) state->data.in_game.key_d = true;      // D Make
            else if (scancode == D_BREAK) state->data.in_game.key_d = false; // D Break
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
            update_car_physics(&state->data.in_game.car,
                               state->data.in_game.key_w,
                               state->data.in_game.key_s,
                               state->data.in_game.key_a,
                               state->data.in_game.key_d);
            /* Center camera on the car */
            car_t *c = &state->data.in_game.car;
            int idx = (int)((c->angle + 11.25) / 22.5) % 16;
            camera_follow((int)c->x + c->sprites[idx]->width / 2,
                          (int)c->y + c->sprites[idx]->height / 2);
            break;
        }
        case STATE_GAME_OVER:
            break;
    }
}
