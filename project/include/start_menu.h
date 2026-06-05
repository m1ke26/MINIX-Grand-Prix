#ifndef _START_MENU_H_
#define _START_MENU_H_

#include "button.h"
#include "font.h"
#include "video-card.h"
#include "vehicles.h"
#include "tracks_pixmaps.h"
#include "sprite.h"

typedef enum {
    START_MENU_ACTION_NONE,
    START_MENU_ACTION_START,
    START_MENU_ACTION_LEADERBOARD,
    START_MENU_ACTION_EXIT,
    START_MENU_ACTION_CAR_LEFT,
    START_MENU_ACTION_CAR_RIGHT,
    START_MENU_ACTION_TRACK_LEFT,
    START_MENU_ACTION_TRACK_RIGHT,
} start_menu_action_t;

#define NUM_CARS   NUM_VEHICLES
#define NUM_TRACKS 3

typedef struct {
  font_t *font;
  button_t *start_btn;
  button_t *leaderboard_btn;
  button_t *exit_btn;
  button_t *car_left;
  button_t *car_right;
  int car_index; // car selected
  sprite_t *car_sprites[NUM_CARS];  // current car sprite
  button_t *track_left;
  button_t *track_right;
  int track_index; // track selected
  sprite_t *track_sprites[NUM_TRACKS]; // current track sprite
  char player_name[16];
  int name_len;
} start_menu_t;

/**
 * @brief Initializes the start menu and its buttons.
 * @param font The font to use for rendering.
 * @param username Optional username to pre-fill; if NULL or empty, shows placeholder.
 */
start_menu_t* start_menu_create(font_t *font, const char *username);

/**
 * @brief Frees start menu resources.
 */
void start_menu_destroy(start_menu_t *sm);

/**
 * @brief Draws all start menu elements.
 * @param sm Pointer to the start menu.
 * @param cursor_x Current mouse x position.
 * @param cursor_y Current mouse y position.
 */
void start_menu_draw(start_menu_t *sm, int cursor_x, int cursor_y);

/**
 * @brief Handles a keyboard input character for the player name field.
 * Appends printable characters to the name or removes the last character on backspace.
 * @param sm Pointer to the start menu.
 * @param key ASCII character to process ('\b' for backspace).
 */

void start_menu_handle_key(start_menu_t *sm, char key);

/**
 * @brief Cycles the track selection by delta (+1 or -1).
 */
void start_menu_change_track(start_menu_t *sm, int delta);

/**
 * @brief Cycles the car selection by delta (+1 or -1).
 */
void start_menu_change_car(start_menu_t *sm, int delta);

/**
 * @brief Processes a left-click at (x, y) and returns the triggered action.
 * @param sm      Pointer to the start menu.
 * @param x       Mouse x coordinate.
 * @param y       Mouse y coordinate.
 * @return The action that should be taken (NONE if no button was hit).
 */
start_menu_action_t start_menu_handle_click(start_menu_t *sm, int x, int y);

#endif /* _START_MENU_H_ */
