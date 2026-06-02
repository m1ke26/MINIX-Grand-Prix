#include "button.h"
#include "video-card.h"
#include <stdlib.h>
#include <string.h>

button_t* button_create(font_t *font, const char *text, int x, int y,
                        uint32_t color, uint32_t hover_color,
                        uint32_t border_color) {
  button_t *b = (button_t *) malloc(sizeof(button_t));
  if (b == NULL) return NULL;

  b->x            = x;
  b->y            = y;
  b->font         = font;
  b->color        = color;
  b->hover_color  = hover_color;
  b->border_color = border_color;
  strncpy(b->text, text, 99);
  b->text[99] = '\0';

  return b;
}

void button_destroy(button_t *b) {
  if (b == NULL) return;
  free(b);
}

void button_draw(button_t *b, bool hover) {
  if (b == NULL) return;

  uint32_t bg = hover ? b->hover_color : b->color;

  /* Outer border */
  vg_buf_draw_rect(b->x, b->y, BTN_W, BTN_H, b->border_color);
  /* Inner fill */
  vg_buf_draw_rect(b->x + BORDER, b->y + BORDER,
                   BTN_W - 2 * BORDER, BTN_H - 2 * BORDER, bg);

  /* Centered label */
  if (b->font != NULL) {
    int text_w = (int)(strlen(b->text) * b->font->tile_size);
    int text_x = b->x + (BTN_W - text_w) / 2;
    int text_y = b->y + (BTN_H - (int)b->font->tile_size) / 2;
    draw_string(b->font, b->text, text_x, text_y, COLOR_BUTTON_TEXT);
  }
}

bool button_is_hovered(button_t *b, int x, int y) {
  if (b == NULL) return false;
  return (x >= b->x && x < b->x + BTN_W &&
          y >= b->y && y < b->y + BTN_H);
}
