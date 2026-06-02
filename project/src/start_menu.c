#include "start_menu.h"
#include "colors.h"
#include <stdlib.h>

start_menu_t* start_menu_create(font_t *font) {
  start_menu_t *sm = (start_menu_t *) malloc(sizeof(start_menu_t));
  if (sm == NULL) return NULL;

  sm->font = font;
  
  // Create buttons centered horizontally
  sm->start_btn = button_create(font, "START", 300, 250,
                                COLOR_BUTTON_DARK_NAVY, COLOR_BUTTON_LIGHT_NAVY, COLOR_BUTTON_BORDER);
  sm->exit_btn  = button_create(font, "EXIT",  300, 370,
                                COLOR_BUTTON_DARK_NAVY, COLOR_BUTTON_LIGHT_NAVY, COLOR_BUTTON_BORDER);

  return sm;
}

void start_menu_destroy(start_menu_t *sm) {
  if (sm == NULL) return;
  button_destroy(sm->start_btn);
  button_destroy(sm->exit_btn);
  free(sm);
}

void start_menu_draw(start_menu_t *sm, int cursor_x, int cursor_y) {
  if (sm == NULL) return;

  // 1. Draw Background (Dark Blue)
  vg_buf_draw_rect(0, 0, 800, 600, COLOR_MENU_BACKGROUND);

  // 2. Draw Game Title
  if (sm->font != NULL)
    draw_string_scaled(sm->font, "MINIX GRAND PRIX", 208, 150, 3, COLOR_MENU_TITLE);

  // 3. Draw Buttons (only if they exist)
  if (sm->start_btn != NULL)
    button_draw(sm->start_btn, button_is_hovered(sm->start_btn, cursor_x, cursor_y));
  if (sm->exit_btn != NULL)
    button_draw(sm->exit_btn, button_is_hovered(sm->exit_btn, cursor_x, cursor_y));

  // 4. Draw cursor
  vg_buf_draw_rect(cursor_x, cursor_y, 8, 8, COLOR_CURSOR);
}
