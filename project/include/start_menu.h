#ifndef _START_MENU_H_
#define _START_MENU_H_

#include "button.h"
#include "font.h"
#include "video-card.h"
#include "car_pixmaps.h"
#include "tracks_pixmaps.h"
#include "sprite.h"

#define NUM_CARS   1
#define NUM_TRACKS 3

typedef struct {
  font_t *font;
  button_t *start_btn;
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
 */
start_menu_t* start_menu_create(font_t *font);

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
 * @brief Cycles the track selection by delta (+1 or -1). Frees old preview, loads new.
 */
void start_menu_change_track(start_menu_t *sm, int delta);

/**
 * @brief Cycles the car selection by delta (+1 or -1).
 */
void start_menu_change_car(start_menu_t *sm, int delta);

#endif /* _START_MENU_H_ */
