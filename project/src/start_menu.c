#include "start_menu.h"
#include "colors.h"
#include <stdlib.h>

start_menu_t* start_menu_create(font_t *font) {
  start_menu_t *sm = (start_menu_t *) malloc(sizeof(start_menu_t));
  if (sm == NULL) return NULL;

  sm->font = font;
  
  // Create buttons centered horizontally
  sm->start_btn = button_create(font, "START", 300, 250, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);
  sm->exit_btn  = button_create(font, "EXIT",  300, 330, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);

  // Car arrows
  sm->car_left  = button_create(NULL, "", 5 ,325, 0x000000, 0x555555, 0x000000, BTN_SHAPE_ARROW_LEFT);
  sm->car_right = button_create(NULL, "", 215, 325, 0x000000, 0x555555, 0x000000, BTN_SHAPE_ARROW_RIGHT);

  // Track arrows
  sm->track_left  = button_create(NULL, "", 555, 325, 0x000000, 0x555555, 0x000000, BTN_SHAPE_ARROW_LEFT);
  sm->track_right = button_create(NULL, "", 765, 325, 0x000000, 0x555555, 0x000000, BTN_SHAPE_ARROW_RIGHT);

  sm->car_index   = 0;
  sm->track_index = 0;
  sm->player_name[0] = '\0';
  sm->name_len = 0;

  // Car Selector
  for (int i = 0; i < NUM_CARS; i++) {
    sm->car_sprites[i] = create_sprite((xpm_map_t) car_xpms[i]);
  }

  // Track Selector
  for (int i = 0; i < NUM_TRACKS; i++) {
    sm->track_sprites[i] = create_sprite((xpm_map_t) track_xpms[i]);
}

  return sm;
}

void start_menu_destroy(start_menu_t *sm) {
  if (sm == NULL) return;
  button_destroy(sm->start_btn);
  button_destroy(sm->exit_btn);
  button_destroy(sm->car_left);
  button_destroy(sm->car_right);
  button_destroy(sm->track_left);
  button_destroy(sm->track_right);

  for (int i = 0; i < NUM_CARS; i++) {
    if (sm->car_sprites[i] != NULL) {
      if (sm->car_sprites[i]->map) free(sm->car_sprites[i]->map);
      free(sm->car_sprites[i]);
    }
  }

  for (int i = 0; i < NUM_TRACKS; i++) {
    if (sm->track_sprites[i] != NULL) {
      if (sm->track_sprites[i]->map) free(sm->track_sprites[i]->map);
      free(sm->track_sprites[i]);
    }
  }

  free(sm);
}

void start_menu_draw(start_menu_t *sm, int cursor_x, int cursor_y) {
  if (sm == NULL) return;

  // 1. Draw Background (Dark Blue)
  vg_buf_draw_rect(0, 0, 800, 600, COLOR_MENU_BACKGROUND);

  // 2. Draw Game Title
  if (sm->font != NULL)
    draw_string_scaled(sm->font, "MINIX GRAND PRIX", 208, 150, 3, 0x0000ff);

  // 3. Draw Buttons (only if they exist)
  if (sm->start_btn != NULL)
    button_draw(sm->start_btn, button_is_hovered(sm->start_btn, cursor_x, cursor_y));
  if (sm->exit_btn != NULL)
    button_draw(sm->exit_btn, button_is_hovered(sm->exit_btn, cursor_x, cursor_y));

  // 4. Draw cursor
  vg_buf_draw_rect(cursor_x, cursor_y, 8, 8, 0xFFFFFF);
}

void start_menu_handle_key(start_menu_t *sm, char key) {
  if (sm == NULL) return;

  if (key == '\b') {
    // backspace delete the last char
    if (sm->name_len > 0) {
      sm->name_len--;
      sm->player_name[sm->name_len] = '\0';
    }
  } else if (sm->name_len < 15) {
    // put the new char if have space
    sm->player_name[sm->name_len] = key;
    sm->name_len++;
    sm->player_name[sm->name_len] = '\0';
  }
}


