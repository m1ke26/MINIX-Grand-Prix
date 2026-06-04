#include "start_menu.h"
#include "colors.h"
#include <stdlib.h>
#include <stdio.h>

start_menu_t* start_menu_create(font_t *font) {
  start_menu_t *sm = (start_menu_t *) malloc(sizeof(start_menu_t));
  if (sm == NULL) return NULL;

  sm->font = font;
  
  // Create buttons centered horizontally
  sm->start_btn = button_create(font, "START", 300, 250, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);
  sm->exit_btn  = button_create(font, "EXIT",  300, 330, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);

  // Car arrows
  sm->car_left  = button_create(NULL, "", 5 ,325, COLOR_BLACK, BTN_DARK_GRAY, COLOR_BLACK, BTN_SHAPE_ARROW_LEFT);
  sm->car_right = button_create(NULL, "", 215, 325, COLOR_BLACK, BTN_DARK_GRAY, COLOR_BLACK, BTN_SHAPE_ARROW_RIGHT);

  // Track arrows
  sm->track_left  = button_create(NULL, "", 555, 325, COLOR_BLACK, BTN_DARK_GRAY, COLOR_BLACK, BTN_SHAPE_ARROW_LEFT);
  sm->track_right = button_create(NULL, "", 765, 325, COLOR_BLACK, BTN_DARK_GRAY, COLOR_BLACK, BTN_SHAPE_ARROW_RIGHT);

  sm->car_index   = 0;
  sm->track_index = 0;
  sm->player_name[0] = '\0';
  sm->name_len = 0;

  // Car Selector (preview uses first sprite of each vehicle)
  for (int i = 0; i < NUM_CARS; i++) {
    sm->car_sprites[i] = create_sprite(vehicle_preview_xpm(i));
  }

  // Track Selector — preload small track previews (takes < 0.03s total)
  for (int i = 0; i < NUM_TRACKS; i++) {
    sm->track_sprites[i] = create_sprite((xpm_map_t) track_preview_xpms[i]);
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
      destroy_sprite(sm->car_sprites[i]);
    }
  }

  for (int i = 0; i < NUM_TRACKS; i++) {
    if (sm->track_sprites[i] != NULL) {
      destroy_sprite(sm->track_sprites[i]);
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
    draw_string_scaled(sm->font, "MINIX GRAND PRIX", 208, 100, 3, COLOR_MENU_TITLE);

  // 3. Draw Buttons (only if they exist)
  if (sm->start_btn != NULL)
    button_draw(sm->start_btn, button_is_hovered(sm->start_btn, cursor_x, cursor_y));
  if (sm->exit_btn != NULL)
    button_draw(sm->exit_btn, button_is_hovered(sm->exit_btn, cursor_x, cursor_y));
  if (sm->car_left != NULL)
    button_draw(sm->car_left, button_is_hovered(sm->car_left, cursor_x,cursor_y));
  if (sm->car_right != NULL)
    button_draw(sm->car_right, button_is_hovered(sm->car_right, cursor_x,cursor_y));
  if (sm->track_left != NULL)
    button_draw(sm->track_left, button_is_hovered(sm->track_left, cursor_x,cursor_y));
  if (sm->track_right != NULL)
    button_draw(sm->track_right, button_is_hovered(sm->track_right, cursor_x,cursor_y));
  
  // 4. Player name
  if (sm->font != NULL) {
    draw_string_scaled(sm->font, "PLAYERNAME:", 320, 470, 2, COLOR_MENU_TEXT);
    draw_string_scaled(sm->font, sm->player_name, 285, 510, 2, COLOR_MENU_TEXT);
    vg_buf_draw_rect(285, 535, 240, 3, COLOR_MENU_UNDERLINE);
  }

  // Car Sprite
  if (sm->car_sprites[sm->car_index] != NULL) {
    sprite_t *s = sm->car_sprites[sm->car_index];
    draw_sprite_scaled_up(s, 90 - s->width  / 2 , 300 - s->height / 2 , 1.5);
  }
  
  // Track Sprite
  if (sm->track_sprites[sm->track_index] != NULL)
    draw_sprite_scaled_down(sm->track_sprites[sm->track_index], 595, 255, 1);

  // Car Label
  char car_label[16];
  sprintf(car_label, "<%s>", vehicle_get(sm->car_index)->label);
  int car_label_len = (int)strlen(car_label);
  int char_width = 8 * 2; // 8px base width * scale 2
  int car_label_x = 120 - (car_label_len * char_width) / 2;
  draw_string_scaled(sm->font, car_label, car_label_x, 420, 2, COLOR_MENU_TEXT);

  // Track Label
  char track_label[16];
  sprintf(track_label, "<TRACK%d>", sm->track_index + 1);
  draw_string_scaled(sm->font, track_label, 600, 420, 2, COLOR_MENU_TEXT);

}

void start_menu_handle_key(start_menu_t *sm, char key) {
  if (sm == NULL) return;

  if (key == '\b') {
    // backspace — apaga o último caracter
    if (sm->name_len > 0) {
      sm->name_len--;
      sm->player_name[sm->name_len] = '\0';
    }
  } else if (sm->name_len < 15) {
    // adiciona o caracter se ainda há espaço
    sm->player_name[sm->name_len] = key;
    sm->name_len++;
    sm->player_name[sm->name_len] = '\0';
  }
}

void start_menu_change_track(start_menu_t *sm, int delta) {
  if (sm == NULL) return;
  sm->track_index = (sm->track_index + delta + NUM_TRACKS) % NUM_TRACKS;
}

void start_menu_change_car(start_menu_t *sm, int delta) {
  if (sm == NULL) return;
  sm->car_index = (sm->car_index + delta + NUM_CARS) % NUM_CARS;
}
