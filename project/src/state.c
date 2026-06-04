#include "speedometer.h"
#include "state.h"
#include "start_menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "car_pixmaps.h"
#include "kbc.h"
#include "pause_menu.h"
#include "hud.h"
#include "cursor.h"
#include "track2_xpm.h"
#include "collision2_xpm.h"


static void reset_game_input(game_input_t *input) {
    if (input == NULL) return;

    input->accelerate = false;
    input->brake = false;
    input->turn_left = false;
    input->turn_right = false;
    input->handbrake = false;
    input->boost = false;
}

static void state_exit_start(state_t *state) {
    start_menu_destroy(state->data.start.menu);
    state->data.start.menu = NULL;
}

static void state_exit_in_game(state_t *state) {
    destroy_car(state->data.in_game.car);
    state->data.in_game.car = NULL;

    pause_menu_destroy(state->data.in_game.pause_menu);
    state->data.in_game.pause_menu = NULL;

    destroy_track(state->data.in_game.track);
    destroy_camera(state->data.in_game.camera);
}

static void state_exit_current(state_t *state) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START:
            state_exit_start(state);
            break;
        case STATE_IN_GAME:
            state_exit_in_game(state);
            break;
        case STATE_GAME_OVER:
            break;
    }
}

state_t *init_state()
{
    state_t *state = malloc(sizeof(state_t));
    if (state == NULL)
        return NULL;

    state->tag = STATE_START;
    state->data.start.cursor_x = 400;
    state->data.start.cursor_y = 300;
    state->data.start.menu = NULL;
    return state;
}

void draw_state(state_t *state)
{
    if (state == NULL)
        return;

    switch (state->tag) {
        case STATE_START:
            start_menu_draw(state->data.start.menu, state->data.start.cursor_x, state->data.start.cursor_y);
            // Draw the selected car sprite as the mouse cursor (always on top)
            if (state->data.start.menu != NULL) {
                int ci = state->data.start.menu->car_index;
                draw_cursor(state->data.start.menu->car_sprites[ci],
                            state->data.start.cursor_x, state->data.start.cursor_y, 2);
            }
            break;
        case STATE_IN_GAME:
            // Draw track and car with camera offset
            draw_track(state->data.in_game.track, state->data.in_game.camera->cam_x, state->data.in_game.camera->cam_y);
            draw_car(state->data.in_game.car, state->data.in_game.camera->cam_x, state->data.in_game.camera->cam_y);
            speedometer_draw(state->data.in_game.car->speed);
            hud_draw_boost_indicator(state->data.in_game.car);
            if(!race_has_started(&state->data.in_game.race)){
                if (race_countdown_started(&state->data.in_game.race)) {
                    hud_draw_race_countdown(state->data.in_game.font,
                                            race_countdown_seconds_left(&state->data.in_game.race));
                }
                else {
                    hud_draw_race_start_prompt(state->data.in_game.font);
                }
                break;
            }
            if(state->data.in_game.pause){
                vg_buf_desaturate(); // Grey out the frozen game world
                pause_menu_draw(state->data.in_game.pause_menu, state->data.in_game.cursor_x, state->data.in_game.cursor_y);
                // Draw the car sprite as cursor on the pause menu too
                if (state->data.in_game.car != NULL)
                    draw_cursor(state->data.in_game.car->sprites[0],
                                state->data.in_game.cursor_x, state->data.in_game.cursor_y, 2);
                break;
            }
            break;
        case STATE_GAME_OVER:
            break;
    }
}

void destroy_state(state_t *state) {
    if (state == NULL) return;
    state_exit_current(state);
    free(state);
}

/* Encapsulates the full setup needed when transitioning to the in-game state */
void state_enter_in_game(state_t *state, font_t *font) {
    if (state == NULL) return;

    state_exit_current(state);
    state->tag = STATE_IN_GAME;

    state->data.in_game.track = create_track((xpm_map_t) track2_xpm, (xpm_map_t) collision2_xpm);
    state->data.in_game.camera = create_camera(state->data.in_game.track->info_trackmap.width, state->data.in_game.track->info_trackmap.height);
    state->data.in_game.car = create_car(1290, 415, 0, 90, (xpm_map_t *) car_xpms);
    if (state->data.in_game.car != NULL) {
        int idx = (int)((state->data.in_game.car->angle + 11.25) / 22.5) % 16;
        follow_camera(state->data.in_game.camera,
                      (int)state->data.in_game.car->x + state->data.in_game.car->sprites[idx]->width / 2,
                      (int)state->data.in_game.car->y + state->data.in_game.car->sprites[idx]->height / 2);
    }
    state->data.in_game.font = font;
    race_init(&state->data.in_game.race);
    reset_game_input(&state->data.in_game.input);
    state->data.in_game.pause = false;
    state->data.in_game.cursor_x = 400;
    state->data.in_game.cursor_y = 300;
    state->data.in_game.pause_menu = pause_menu_create(font);
    speedometer_init();
}

/* Tears down all in-game resources and transitions back to the start menu */
void state_enter_start(state_t *state, font_t *font, int cursor_x, int cursor_y) {
    if (state == NULL) return;

    state_exit_current(state);
    state->tag = STATE_START;
    state->data.start.cursor_x = cursor_x;
    state->data.start.cursor_y = cursor_y;
    state->data.start.menu = start_menu_create(font);
}

void handle_mouse_event(state_t *state, struct packet *pp, int cursor_x, int cursor_y)
{
    if (state == NULL)
        return;

    switch (state->tag) {
        case STATE_START:
            state->data.start.cursor_x = cursor_x;
            state->data.start.cursor_y = cursor_y;

            // Check for Left Click
            if (pp->lb) {
                if (button_is_hovered(state->data.start.menu->start_btn, cursor_x, cursor_y)) {
                    font_t *font = state->data.start.menu->font;
                    state_enter_in_game(state, font);
                }
                else if (button_is_hovered(state->data.start.menu->exit_btn, cursor_x, cursor_y)) {
                    running = false;
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
            if (state->data.start.menu != NULL) {
                char c = kbd_scancode_to_char(scancode);
                if (c != 0)
                    start_menu_handle_key(state->data.start.menu, c);
            }
            break;
        case STATE_IN_GAME:
            if(!race_has_started(&state->data.in_game.race)){
                if (scancode == ENTER_MAKE) race_start_countdown(&state->data.in_game.race);
                break;
            }
            if (scancode == ESC_MAKE) {
                state->data.in_game.pause = !state->data.in_game.pause;
                break;
            }
            if(!state->data.in_game.pause) {
                if (scancode == W_MAKE) state->data.in_game.input.accelerate = true;
                else if (scancode == W_BREAK) state->data.in_game.input.accelerate = false;

                if (scancode == S_MAKE) state->data.in_game.input.brake = true;
                else if (scancode == S_BREAK) state->data.in_game.input.brake = false;

                if (scancode == A_MAKE) state->data.in_game.input.turn_left = true;
                else if (scancode == A_BREAK) state->data.in_game.input.turn_left = false;

                if (scancode == D_MAKE) state->data.in_game.input.turn_right = true;
                else if (scancode == D_BREAK) state->data.in_game.input.turn_right = false;

                if (scancode == SPACE_MAKE) state->data.in_game.input.handbrake = true;
                else if (scancode == SPACE_BREAK) state->data.in_game.input.handbrake = false;

                if (scancode == SHIFT_MAKE) state->data.in_game.input.boost = true;
                else if (scancode == SHIFT_BREAK) state->data.in_game.input.boost = false;
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
            if (!race_has_started(&state->data.in_game.race)) {
                race_update(&state->data.in_game.race);
                break;
            }
            if (state->data.in_game.pause) break;
            update_car_physics(state->data.in_game.car, &state->data.in_game.input, state->data.in_game.track);
            car_t *c = state->data.in_game.car;
            int idx = (int)((c->angle + 11.25) / 22.5) % 16;
            follow_camera(state->data.in_game.camera,
                          (int)c->x + c->sprites[idx]->width / 2,
                          (int)c->y + c->sprites[idx]->height / 2);
            break;
        }
        case STATE_GAME_OVER:
            break;
    }
}
