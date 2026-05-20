#include "start_menu.h"
#include <stdlib.h>

start_menu_t* start_menu_create(font_t *font, xpm_map_t btn_normal, xpm_map_t btn_hover) {
  start_menu_t *sm = (start_menu_t *) malloc(sizeof(start_menu_t));
  if (sm == NULL) return NULL;

  sm->font = font;
  
  // Create buttons centered horizontally
  sm->start_btn = button_create(font, "START", 300, 250, btn_normal, btn_hover);
  sm->exit_btn  = button_create(font, "EXIT", 300, 370, btn_normal, btn_hover);

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
  vg_buf_draw_rect(0, 0, 800, 600, 0xB0BEC5);

  // 2. Draw Game Title
  if (sm->font != NULL)
    draw_string(sm->font, "MINIX GRAND PRIX", 310, 150);

  // 3. Draw Buttons (only if they exist)
  if (sm->start_btn != NULL)
    button_draw(sm->start_btn, button_is_hovered(sm->start_btn, cursor_x, cursor_y));
  if (sm->exit_btn != NULL)
    button_draw(sm->exit_btn, button_is_hovered(sm->exit_btn, cursor_x, cursor_y));

  // 4. Draw cursor
  vg_buf_draw_rect(cursor_x, cursor_y, 8, 8, 0xFFFFFF);
}
