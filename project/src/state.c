#include <stdio.h>
#include <string.h>
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
#include "leaderboard_menu.h"


static void reset_game_input(game_input_t *input)
{
    if (input == NULL)
        return;

    input->accelerate = false;
    input->brake = false;
    input->turn_left = false;
    input->turn_right = false;
    input->handbrake = false;
    input->boost = false;
}

static void state_exit_start(state_t *state)
{
    start_menu_destroy(state->data.start.menu);
    state->data.start.menu = NULL;
}

static void state_exit_in_game(state_t *state)
{
    destroy_car(state->data.in_game.car);
    state->data.in_game.car = NULL;

    pause_menu_destroy(state->data.in_game.pause_menu);
    state->data.in_game.pause_menu = NULL;

    destroy_track(state->data.in_game.track);
    state->data.in_game.track = NULL;
    destroy_camera(state->data.in_game.camera);
    state->data.in_game.camera = NULL;
}

static void state_exit_current(state_t *state)
{
    if (state == NULL)
        return;

    switch (state->tag)
    {
    case STATE_START:
        state_exit_start(state);
        break;
    case STATE_LEADERBOARD:
        if (state->data.leaderboard.menu != NULL) {
            leaderboard_menu_destroy(state->data.leaderboard.menu);
            state->data.leaderboard.menu = NULL;
        }
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

    switch (state->tag)
    {
    case STATE_START:
        start_menu_draw(state->data.start.menu, state->cursor->x, state->cursor->y);
        // Draw the selected car sprite as the mouse cursor (always on top)
        if (state->data.start.menu != NULL && state->cursor != NULL)
        {
            int ci = state->data.start.menu->car_index;
            update_cursor_sprite(state->cursor, state->data.start.menu->car_sprites[ci]);
            draw_cursor(state->cursor);
        }
        break;
    case STATE_LEADERBOARD:
        if (state->data.leaderboard.menu != NULL) {
            leaderboard_menu_draw(state->data.leaderboard.menu);
        }
        break;
    case STATE_LOADING:
        vg_buf_clear();
        vg_buf_draw_rect(0, 0, 800, 600, COLOR_MENU_BACKGROUND); // Dark blueish background
        if (state->data.loading.font != NULL)
        {
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

        if (!race_has_started(&state->data.in_game.race))
        {
            if (race_countdown_started(&state->data.in_game.race))
            {
                hud_draw_race_countdown(state->data.in_game.font,
                                        race_countdown_seconds_left(&state->data.in_game.race));
            }
            else
            {
                hud_draw_race_start_prompt(state->data.in_game.font);
                hud_draw_controls_panel(state->data.in_game.font);
            }
            break;
        }
        
        hud_draw_lap_counter(state->data.in_game.font, state->data.in_game.race.current_lap, state->data.in_game.race.total_laps);
        hud_draw_timer(state->data.in_game.font, state->data.in_game.race.seconds_elapsed);
        
        if (state->data.in_game.pause)
        {
            vg_buf_desaturate(); // Grey out the frozen game world
            pause_menu_draw(state->data.in_game.pause_menu, state->cursor->x, state->cursor->y);
            // Draw the car sprite as cursor on the pause menu too
            if (state->data.in_game.car != NULL && state->cursor != NULL)
            {
                update_cursor_sprite(state->cursor, state->data.in_game.car->sprites[0]);
                draw_cursor(state->cursor);
            }
            break;
        }
        break;
    case STATE_GAME_OVER:
        if (state->data.game_over.menu != NULL)
        {
            finish_menu_draw(state->data.game_over.menu);
        }
        break;
    }
}

void destroy_state(state_t *state)
{
    if (state == NULL)
        return;
    state_exit_current(state);
    if (state->cursor != NULL)
    {
        destroy_cursor(state->cursor);
        state->cursor = NULL;
    }
    free(state);
}

void state_enter_loading(state_t *state, font_t *font)
{
    if (state == NULL)
        return;
    int track_idx = state->data.start.menu->track_index;
    int car_idx = state->data.start.menu->car_index;
    const char *username = state->data.start.menu->player_name;

    state_exit_current(state);
    state->tag = STATE_LOADING;
    state->data.loading.font = font;
    state->data.loading.track_index = track_idx;
    state->data.loading.car_index = car_idx;

    // Store username
    if (username != NULL && username[0] != '\0')
    {
        strncpy(state->data.loading.username, username, sizeof(state->data.loading.username) - 1);
        state->data.loading.username[sizeof(state->data.loading.username) - 1] = '\0';
    }
    else
    {
        state->data.loading.username[0] = '\0';
    }
    state->data.loading.drawn = false;
}

void state_enter_leaderboard(state_t *state, font_t *font, const char *username)
{
    if (state == NULL)
        return;

    state_exit_current(state);
    state->tag = STATE_LEADERBOARD;
    state->data.leaderboard.menu = leaderboard_menu_create(font, username);
}

static void state_enter_in_game(state_t *state, font_t *font, int track_idx, int car_idx, const char *username)
{
    if (state == NULL)
        return;

    // Copy username to local BEFORE touching state->data (union overlap!)
    char local_username[16] = {0};
    if (username != NULL)
    {
        strncpy(local_username, username, sizeof(local_username) - 1);
    }

    state->tag = STATE_IN_GAME;

    const vehicle_def_t *vehicle = vehicle_get(car_idx);

    state->data.in_game.track = create_track((xpm_map_t)track_xpms[track_idx], (xpm_map_t)collision_xpms[track_idx]);
    state->data.in_game.camera = create_camera(state->data.in_game.track->info_trackmap.width, state->data.in_game.track->info_trackmap.height);

    int spawn_x = 1290;
    int spawn_y = 415;
    state->data.in_game.car = create_car(spawn_x, spawn_y, 0, 90, vehicle->xpms, vehicle->num_sprites);
    if (state->data.in_game.car != NULL)
    {
        int idx = car_sprite_index(state->data.in_game.car);
        int car_w = state->data.in_game.car->sprites[idx]->width;
        int car_h = state->data.in_game.car->sprites[idx]->height;

        if (track_find_start_spawn(state->data.in_game.track, car_w, car_h, &spawn_x, &spawn_y))
        {
            state->data.in_game.car->x = spawn_x;
            state->data.in_game.car->y = spawn_y;
        }

        follow_camera(state->data.in_game.camera,
                      (int)state->data.in_game.car->x + car_w / 2,
                      (int)state->data.in_game.car->y + car_h / 2);
    }
    state->data.in_game.font = font;
    state->data.in_game.track_index = track_idx;

    // Now safe — copy from local, not from the union that was just overwritten
    strncpy(state->data.in_game.username, local_username, sizeof(state->data.in_game.username) - 1);
    state->data.in_game.username[sizeof(state->data.in_game.username) - 1] = '\0';

    race_init(&state->data.in_game.race);
    reset_game_input(&state->data.in_game.input);
    state->data.in_game.pause = false;
    state->data.in_game.pause_menu = pause_menu_create(font);
    speedometer_init();
}

/* Tears down all in-game resources and transitions back to the start menu */
void state_enter_start(state_t *state, font_t *font, const char *username)
{
    if (state == NULL) return;

    char local_username[16] = {0};
    if (username != NULL)
    {
        strncpy(local_username, username, sizeof(local_username) - 1);
        local_username[sizeof(local_username) - 1] = '\0';
    }

    state_exit_current(state);
    state->tag = STATE_START;
    state->data.start.menu = start_menu_create(font, local_username);
}

void state_enter_game_over(state_t *state, font_t *font, const race_t *race)
{
    if (state == NULL)
        return;

    // Save BEFORE exit
    char saved_username[16] = {0};
    int saved_track_index = 0;
    if (state->tag == STATE_IN_GAME)
    {
        strncpy(saved_username, state->data.in_game.username, sizeof(saved_username) - 1);
        saved_track_index = state->data.in_game.track_index;
    }

    state_exit_current(state);
    state->tag = STATE_GAME_OVER;
    state->data.game_over.menu = finish_menu_create(font, race);

    // Store username for later use when returning to menu
    strncpy(state->data.game_over.username, saved_username, sizeof(state->data.game_over.username) - 1);
    state->data.game_over.username[sizeof(state->data.game_over.username) - 1] = '\0';

    // Now safe to use the saved values
    if (race != NULL && saved_username[0] != '\0')
    {
        rtc_date date;
        rtc_read_date(&date);
        leaderboard_save_time(saved_username, race->ticks_elapsed, saved_track_index, date);
    }
}

void handle_mouse_event(state_t *state, struct packet *pp, int cursor_x, int cursor_y)
{
    if (state == NULL)
        return;

    if (!pp->lb)
        return;

    switch (state->tag)
    {
    case STATE_START:
    {
        start_menu_t *menu = state->data.start.menu;
        if (menu == NULL) break;

        switch (start_menu_handle_click(menu, cursor_x, cursor_y))
        {
        case START_MENU_ACTION_START:
            state_enter_loading(state, menu->font);
            break;
        case START_MENU_ACTION_LEADERBOARD:
            state_enter_leaderboard(state, menu->font, menu->player_name);
            break;
        case START_MENU_ACTION_EXIT:
            running = false;
            break;
        case START_MENU_ACTION_CAR_LEFT:
            start_menu_change_car(menu, -1);
            break;
        case START_MENU_ACTION_CAR_RIGHT:
            start_menu_change_car(menu, 1);
            break;
        case START_MENU_ACTION_TRACK_LEFT:
            start_menu_change_track(menu, -1);
            break;
        case START_MENU_ACTION_TRACK_RIGHT:
            start_menu_change_track(menu, 1);
            break;
        case START_MENU_ACTION_NONE:
        default:
            break;
        }
        break;
    }

    case STATE_IN_GAME:
        if (!state->data.in_game.pause || state->data.in_game.pause_menu == NULL)
            break;

        switch (pause_menu_handle_click(state->data.in_game.pause_menu, cursor_x, cursor_y))
        {
        case PAUSE_MENU_ACTION_RESUME:
            state->data.in_game.pause = false;
            break;
        case PAUSE_MENU_ACTION_EXIT:
            state_enter_start(state,
                              state->data.in_game.pause_menu->font,
                              state->data.in_game.username);
            break;
        case PAUSE_MENU_ACTION_NONE:
        default:
            break;
        }
        break;

    case STATE_LEADERBOARD:
    case STATE_LOADING:
    case STATE_GAME_OVER:
    default:
        break;
    }
}

static void in_game_handle_key(state_t *state, uint8_t scancode)
{
    race_t       *race  = &state->data.in_game.race;
    game_input_t *input = &state->data.in_game.input;
    bool         *pause = &state->data.in_game.pause;

    /* Before race starts: only ENTER kicks off the countdown */
    if (!race_has_started(race)) {
        if (scancode == ENTER_MAKE)
            race_start_countdown(race);
        return;
    }

    /* ESC toggles pause regardless of current pause state */
    if (scancode == ESC_MAKE) {
        *pause = !*pause;
        return;
    }

    /* Driving controls — only when not paused */
    if (*pause) return;

    if      (scancode == W_MAKE)      input->accelerate = true;
    else if (scancode == W_BREAK)     input->accelerate = false;
    if      (scancode == S_MAKE)      input->brake      = true;
    else if (scancode == S_BREAK)     input->brake      = false;
    if      (scancode == A_MAKE)      input->turn_left  = true;
    else if (scancode == A_BREAK)     input->turn_left  = false;
    if      (scancode == D_MAKE)      input->turn_right = true;
    else if (scancode == D_BREAK)     input->turn_right = false;
    if      (scancode == SPACE_MAKE)  input->handbrake  = true;
    else if (scancode == SPACE_BREAK) input->handbrake  = false;
    if      (scancode == SHIFT_MAKE)  input->boost      = true;
    else if (scancode == SHIFT_BREAK) input->boost      = false;
}

void handle_kbd_event(state_t *state, uint8_t scancode)
{
    if (state == NULL)
        return;

    switch (state->tag)
    {
    case STATE_START:
        if (scancode == 0xE0 || scancode == 0xE1)
            break;
        char c = kbd_scancode_to_char(scancode);
        if (c != 0)
            start_menu_handle_key(state->data.start.menu, c);
        break;

    case STATE_LEADERBOARD:
        if (state->data.leaderboard.menu != NULL &&
            leaderboard_menu_handle_key(state->data.leaderboard.menu, scancode))
        {
            state_enter_start(state,
                              state->data.leaderboard.menu->font,
                              state->data.leaderboard.menu->username);
        }
        break;

    case STATE_IN_GAME:
        in_game_handle_key(state, scancode);
        break;

    case STATE_GAME_OVER:
        if (state->data.game_over.menu != NULL &&
            finish_menu_handle_key(state->data.game_over.menu, scancode))
        {
            state_enter_start(state,
                              state->data.game_over.menu->font,
                              state->data.game_over.username);
        }
        break;

    case STATE_LOADING:
    default:
        break;
    }
}

void update_state(state_t *state)
{
    if (state == NULL)
        return;

    switch (state->tag)
    {
    case STATE_START:
        break;
    case STATE_LOADING:
        if (state->data.loading.drawn)
        {
            state_enter_in_game(state, state->data.loading.font,
                                state->data.loading.track_index,
                                state->data.loading.car_index,
                                state->data.loading.username);
        }
        break;
    case STATE_IN_GAME:
    {
        if (state->data.in_game.pause)
            break; // Skip physics and camera while paused

        // Update the race (either countdown ticks or elapsed race ticks)
        race_update(&state->data.in_game.race);

        if (!race_countdown_started(&state->data.in_game.race))
            break;

        car_t *c = state->data.in_game.car;
        if (c == NULL) break;
        update_car_physics(c, &state->data.in_game.input, state->data.in_game.track);

        int idx = car_sprite_index(c);

        if (race_has_started(&state->data.in_game.race))
        {
            race_check_checkpoints(&state->data.in_game.race, state->data.in_game.track,
                                   c->x, c->y, c->sprites[idx]->width, c->sprites[idx]->height);
        }

        if (race_is_finished(&state->data.in_game.race))
        {
            state_enter_game_over(state, state->data.in_game.font, &state->data.in_game.race);
            break;
        }

        follow_camera(state->data.in_game.camera,
                      (int)c->x + c->sprites[idx]->width / 2,
                      (int)c->y + c->sprites[idx]->height / 2);

        break;
    }
    case STATE_GAME_OVER:
        break;
    
    case STATE_LEADERBOARD:
        break;
}
}
