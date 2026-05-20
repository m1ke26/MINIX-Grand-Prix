#include "button.h"
#include "video-card.h"
#include <stdlib.h>
#include <string.h>



button_t* button_create(font_t *font, const char *text, int x, int y,
                        xpm_map_t normal_xpm, xpm_map_t hover_xpm) {

  button_t *b = (button_t *) malloc(sizeof(button_t));
  if (b == NULL) return NULL;

  b->sp       = create_sprite(normal_xpm);
  b->hover_sp = create_sprite(hover_xpm); 

  if (b->sp == NULL || b->hover_sp == NULL) {
    button_destroy(b);
    return NULL;
  }

  b->x        = x;
  b->y        = y;
  b->font     = font;
  strncpy(b->text, text, 99);
  b->text[99] = '\0';

  return b;
}

void button_destroy(button_t *b) {
  if (b == NULL) return;
  if (b->sp != NULL) {
    destroy_sprite(b->sp);
  }
  if (b->hover_sp != NULL) {
    destroy_sprite(b->hover_sp);
  }
  
  free(b);
}

void button_draw(button_t *b, bool hover) {
  if (b == NULL) return;

  if(hover) sprite_draw(b->hover_sp, b->x, b->y);
  else      sprite_draw(b->sp, b->x, b->y);

  /* Centered text */
  if (b->font != NULL) {
    int text_w = (int)(strlen(b->text) * b->font->tile_size);
    int text_x = b->x + (BTN_W - text_w) / 2;
    int text_y = b->y + (BTN_H - (int)b->font->tile_size) / 2;
    draw_string(b->font, b->text, text_x, text_y);
  }
}

bool button_is_hovered(button_t *b, int x, int y) {
  if (b == NULL) return false;
  return (x >= b->x && x < b->x + BTN_W &&
          y >= b->y && y < b->y + BTN_H);
}
