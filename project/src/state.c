#include <stdio.h>
#include "speedometer.h"
#include "state.h"
#include "finish_menu.h"
#include "colors.h"
#include "start_menu.h"
#include "car.h"
#include "track.h"
#include "camera.h"
#include "vehicles.h"
#include "kbc.h"
#include "pause_menu.h"
#include "hud.h"
#include "cursor.h"
#include "rtc.h"
#include "tracks_pixmaps.h"
#include "leaderboard.h"
#include <string.h>
#include <stdio.h>



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
            finish_menu_destroy(state->data.game_over.menu);
            state->data.game_over.menu = NULL;
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
            draw_car(state->data.in_game.car, state->data.in_game.camera->cam_x, state->data.in_game.camera->cam_y, car_sprite_index(state->data.in_game.car));
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
<<<<<<< HEAD
        case STATE_GAME_OVER:
            if (state->data.game_over.menu != NULL) {
                finish_menu_draw(state->data.game_over.menu);
            }
=======
        case STATE_GAME_OVER: {
>>>>>>> leaderboard-storing
            break;
        }
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
    int car_idx = state->data.start.menu->car_index;
    const char *username = state->data.start.menu->player_name;

    state_exit_current(state);
    state->tag = STATE_LOADING;
    state->data.loading.font = font;
    state->data.loading.track_index = track_idx;
    state->data.loading.car_index = car_idx;
    
    // Store username
    if (username != NULL && username[0] != '\0') {
        strncpy(state->data.loading.username, username, sizeof(state->data.loading.username) - 1);
        state->data.loading.username[sizeof(state->data.loading.username) - 1] = '\0';
    } else {
        strcpy(state->data.loading.username, "Player");
    }
    
    state->data.loading.drawn = false;
}

static void state_enter_in_game(state_t *state, font_t *font, int track_idx, int car_idx, const char *username) {
    if (state == NULL) return;

    state->tag = STATE_IN_GAME;

    const vehicle_def_t *vehicle = vehicle_get(car_idx);

    state->data.in_game.track = create_track((xpm_map_t) track_xpms[track_idx], (xpm_map_t) collision_xpms[track_idx]);
    state->data.in_game.camera = create_camera(state->data.in_game.track->info_trackmap.width, state->data.in_game.track->info_trackmap.height);

    int spawn_x = 1290;
    int spawn_y = 415;
    state->data.in_game.car = create_car(spawn_x, spawn_y, 0, 90, vehicle->xpms, vehicle->num_sprites);
    if (state->data.in_game.car != NULL) {
        int idx = car_sprite_index(state->data.in_game.car);
        int car_w = state->data.in_game.car->sprites[idx]->width;
        int car_h = state->data.in_game.car->sprites[idx]->height;

        if (track_find_start_spawn(state->data.in_game.track, car_w, car_h, &spawn_x, &spawn_y)) {
            state->data.in_game.car->x = spawn_x;
            state->data.in_game.car->y = spawn_y;
        }

        follow_camera(state->data.in_game.camera,
                      (int)state->data.in_game.car->x + car_w / 2,
                      (int)state->data.in_game.car->y + car_h / 2);
    }
    state->data.in_game.font = font;
    state->data.in_game.track_index = track_idx;
    
    // Store username
    if (username != NULL) {
        strncpy(state->data.in_game.username, username, sizeof(state->data.in_game.username) - 1);
        state->data.in_game.username[sizeof(state->data.in_game.username) - 1] = '\0';
    } else {
        state->data.in_game.username[0] = '\0';
    }
    
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

void state_enter_game_over(state_t *state, font_t *font, unsigned final_time, const char *username, int track_index) {
    if (state == NULL) return;

    state_exit_current(state);
    state->tag = STATE_GAME_OVER;
    state->data.game_over.final_time = final_time;
    rtc_read_date(&state->data.game_over.date);
    
    // Store username and track
    if (username != NULL) {
        strncpy(state->data.game_over.username, username, sizeof(state->data.game_over.username) - 1);
        state->data.game_over.username[sizeof(state->data.game_over.username) - 1] = '\0';
    } else {
        state->data.game_over.username[0] = '\0';
    }
    state->data.game_over.track_index = track_index;
    
    // Save time to file
    if (username != NULL) {
        leaderboard_save_time(username, final_time, track_index, state->data.game_over.date);
    }
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
            if (state->data.game_over.menu != NULL && finish_menu_handle_key(state->data.game_over.menu, scancode)) {
                state_enter_start(state, state->data.game_over.menu->font);
            }
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
                state_enter_in_game(state, state->data.loading.font,
                                    state->data.loading.track_index,
                                    state->data.loading.car_index,
                                    state->data.loading.username);
            }
            break;
        case STATE_IN_GAME: {
            if (state->data.in_game.pause) break; // Skip physics and camera while paused
            
            // Update the race (either countdown ticks or elapsed race ticks)
            race_update(&state->data.in_game.race);

            if (!race_countdown_started(&state->data.in_game.race))
                break;

            update_car_physics(state->data.in_game.car, &state->data.in_game.input, state->data.in_game.track);

            car_t *c = state->data.in_game.car;
            if (c == NULL) break;
            int idx = car_sprite_index(c);

            if (race_has_started(&state->data.in_game.race)) {
                race_check_checkpoints(&state->data.in_game.race, state->data.in_game.track,
                                       c->x, c->y, c->sprites[idx]->width, c->sprites[idx]->height);
            }

            if (race_is_finished(&state->data.in_game.race)) {
                state_enter_game_over(state, state->data.in_game.font, state->data.in_game.race.seconds_elapsed,
                                      state->data.in_game.username, state->data.in_game.track_index);
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
