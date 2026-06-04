#include "speedometer.h"
#include "state.h"
#include "colors.h"
#include "start_menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "car_pixmaps.h"
#include "police_pixmaps.h"
#include "kbc.h"
#include "pause_menu.h"
#include "hud.h"
#include "cursor.h"
#include "rtc.h"
#include "tracks_pixmaps.h"



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
        case STATE_LOADING:
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
    state->cursor = create_cursor(NULL, 400, 300, 2);
    state->data.start.menu = NULL;
    return state;
}

void draw_state(state_t *state)
{
    if (state == NULL)
        return;

    switch (state->tag) {
        case STATE_START:
            start_menu_draw(state->data.start.menu, state->cursor->x, state->cursor->y);
            // Draw the selected car sprite as the mouse cursor (always on top)
            if (state->data.start.menu != NULL && state->cursor != NULL) {
                int ci = state->data.start.menu->car_index;
                update_cursor_sprite(state->cursor, state->data.start.menu->car_sprites[ci]);
                draw_cursor(state->cursor);
            }
            break;
        case STATE_LOADING:
            vg_buf_clear();
            vg_buf_draw_rect(0, 0, 800, 600, COLOR_MENU_BACKGROUND); // Dark blueish background
            if (state->data.loading.font != NULL) {
                draw_string_scaled(state->data.loading.font, "LOADING MAP...", 260, 280, 3, COLOR_MENU_TEXT);
            }
            state->data.loading.drawn = true;
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
            if(race_has_started( &state->data.in_game.race)){
                hud_draw_lap_counter(state->data.in_game.font, state->data.in_game.race.current_lap, state->data.in_game.race.total_laps);
                hud_draw_timer(state->data.in_game.font, state->data.in_game.race.seconds_elapsed);
            }
            if(state->data.in_game.pause){
                vg_buf_desaturate(); // Grey out the frozen game world
                pause_menu_draw(state->data.in_game.pause_menu, state->cursor->x, state->cursor->y);
                // Draw the car sprite as cursor on the pause menu too
                if (state->data.in_game.car != NULL && state->cursor != NULL) {
                    update_cursor_sprite(state->cursor, state->data.in_game.car->sprites[0]);
                    draw_cursor(state->cursor);
                }
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
    if (state->cursor != NULL) {
        destroy_cursor(state->cursor);
        state->cursor = NULL;
    }
    free(state);
}

void state_enter_loading(state_t *state, font_t *font) {
    if (state == NULL) return;
    int track_idx = state->data.start.menu->track_index;

    state_exit_current(state);
    state->tag = STATE_LOADING;
    state->data.loading.font = font;
    state->data.loading.track_index = track_idx;
    state->data.loading.drawn = false;
}

static void state_enter_in_game(state_t *state, font_t *font, int track_idx) {
    if (state == NULL) return;

    state->tag = STATE_IN_GAME;

    state->data.in_game.track = create_track((xpm_map_t) track_xpms[track_idx], (xpm_map_t) collision_xpms[track_idx]);
    state->data.in_game.camera = create_camera(state->data.in_game.track->info_trackmap.width, state->data.in_game.track->info_trackmap.height);
    state->data.in_game.car = create_car(1290, 415, 0, 90, (xpm_map_t *) car_xpms);
    if (state->data.in_game.car != NULL) {
        int idx = (int)((state->data.in_game.car->angle + 11.25) / 22.5) % 16;
        follow_camera(state->data.in_game.camera,
                      (int)state->data.in_game.car->x + car_w / 2,
                      (int)state->data.in_game.car->y + car_h / 2);
    }
    state->data.in_game.font = font;
    race_init(&state->data.in_game.race);
    reset_game_input(&state->data.in_game.input);
    state->data.in_game.pause = false;
    state->data.in_game.pause_menu = pause_menu_create(font);
    speedometer_init();
}

/* Tears down all in-game resources and transitions back to the start menu */
void state_enter_start(state_t *state, font_t *font) {
    if (state == NULL) return;

    state_exit_current(state);
    state->tag = STATE_START;
    state->data.start.menu = start_menu_create(font);
}

void state_enter_game_over(state_t *state, font_t *font, unsigned final_time) {
    if (state == NULL) return;

    state_exit_current(state);
    state->tag = STATE_GAME_OVER;
    state->data.game_over.final_time = final_time;
    rtc_read_date(&state->data.game_over.date);
}

void handle_mouse_event(state_t *state, struct packet *pp, int cursor_x, int cursor_y) {
    if (state == NULL) return;

    switch (state->tag) {
        case STATE_START: {
            start_menu_t *menu = state->data.start.menu;
            if (menu == NULL) break;

            if (!pp->lb) break;

            if (button_is_hovered(menu->start_btn, cursor_x, cursor_y)) {
                state_enter_loading(state, menu->font);
            }
            else if (button_is_hovered(menu->exit_btn, cursor_x, cursor_y)) {
                running = false;
            }
            else if (button_is_hovered(menu->car_left, cursor_x, cursor_y)) {
                start_menu_change_car(menu, -1);
            }
            else if (button_is_hovered(menu->car_right, cursor_x, cursor_y)) {
                start_menu_change_car(menu, 1);
            }
            else if (button_is_hovered(menu->track_left, cursor_x, cursor_y)) {
                start_menu_change_track(menu, -1);
            }
            else if (button_is_hovered(menu->track_right, cursor_x, cursor_y)) {
                start_menu_change_track(menu, 1);
            }
            break;
        }

        case STATE_IN_GAME:
            if(pp->lb && state->data.in_game.pause && state->data.in_game.pause_menu != NULL) {
                if(button_is_hovered(state->data.in_game.pause_menu->resume_btn, cursor_x, cursor_y)) {
                    state->data.in_game.pause = false;
                }
                else if (button_is_hovered(state->data.in_game.pause_menu->exit_btn, cursor_x, cursor_y)) {
                    font_t *font = state->data.in_game.pause_menu->font;
                    state_enter_start(state, font);
                }
            }
            break;

        case STATE_LOADING:
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
        case STATE_LOADING:
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
        case STATE_LOADING:
            if (state->data.loading.drawn) {
                state_enter_in_game(state, state->data.loading.font, state->data.loading.track_index);
            }
            break;
        case STATE_IN_GAME: {
            if (state->data.in_game.pause) break; // Skip physics and camera while paused
            
            // Update the race (either countdown ticks or elapsed race ticks)
            race_update(&state->data.in_game.race);

            // Skip physics and checkpoint updates if the countdown is still running
            if (!race_has_started(&state->data.in_game.race)) break;

            update_car_physics(state->data.in_game.car, &state->data.in_game.input, state->data.in_game.track);
            car_t *c = state->data.in_game.car;
            int idx = (int)((c->angle + 11.25) / 22.5) % 16;
            
            race_check_checkpoints(&state->data.in_game.race, state->data.in_game.track,
                                   c->x, c->y, c->sprites[idx]->width, c->sprites[idx]->height);

            if (race_is_finished(&state->data.in_game.race)) {
                state_enter_game_over(state, state->data.in_game.font, state->data.in_game.race.seconds_elapsed);
                break;
            }

            follow_camera(state->data.in_game.camera,
                          (int)c->x + c->sprites[idx]->width / 2,
                          (int)c->y + c->sprites[idx]->height / 2);

            break;
        }
        case STATE_GAME_OVER:
            break;
    }
}
