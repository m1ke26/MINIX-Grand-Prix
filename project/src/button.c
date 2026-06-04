#include "button.h"
#include "video-card.h"
#include <stdlib.h>
#include <string.h>

button_t* button_create(font_t *font, const char *text, int x, int y,
                        button_color_t color, button_color_t hover_color,
                        button_color_t border_color,
                        button_shape_t shape) {
  button_t *b = (button_t *) malloc(sizeof(button_t));
  if (b == NULL) return NULL;

  b->x            = x;
  b->y            = y;
  b->font         = font;
  b->color        = color;
  b->hover_color  = hover_color;
  b->border_color = border_color;
  b->shape        = shape;
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

  if (b-> shape == BTN_SHAPE_RECT){

    /* Outer border */
    vg_buf_draw_rect(b->x, b->y, BTN_W, BTN_H, (uint32_t)b->border_color);
    /* Inner fill */
    vg_buf_draw_rect(b->x + BORDER, b->y + BORDER,
                   BTN_W - 2 * BORDER, BTN_H - 2 * BORDER, bg);

    /* Centered label */
    if (b->font != NULL) {
      int text_w = (int)(strlen(b->text) * b->font->tile_size);
      int text_x = b->x + (BTN_W - text_w) / 2;
      int text_y = b->y + (BTN_H - (int)b->font->tile_size) / 2;
      draw_string(b->font, b->text, text_x, text_y, (uint32_t)BTN_WHITE);
    }
  }
  else {
    int s = BTN_H / 2;
    int center_y = b->y; 

    if (b->shape == BTN_SHAPE_ARROW_RIGHT) {
      for (int i = 0; i < s; i++) {
        int h = i * 2 + 1;
        int top_y = center_y - h / 2;  
        vg_buf_draw_rect(b->x + (s - 1 - i), top_y, 1, h, bg);
      }
    } else {
      for (int i = 0; i < s; i++) {
        int h = i * 2 + 1;
        int top_y = center_y - h / 2;
        vg_buf_draw_rect(b->x + i, top_y, 1, h, bg);
      }
    }
  }
}
    
bool button_is_hovered(button_t *b, int x, int y) {
  if (b == NULL) return false;
  if (b->shape == BTN_SHAPE_RECT) {
    return (x >= b->x && x < b->x + BTN_W &&
            y >= b->y && y < b->y + BTN_H);
  } else {
    int s = BTN_H / 2;
    return (x >= b->x     && x <= b->x + s &&
            y >= b->y - s && y <= b->y + s);
  }
}

