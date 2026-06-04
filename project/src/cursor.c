#include "cursor.h"
#include <stdlib.h>
#include "sprite.h"

cursor_t* create_cursor(sprite_t *sprite, int x, int y, int scale) {
  cursor_t *cursor = malloc(sizeof(cursor_t));
  if (cursor == NULL) return NULL;

  cursor->sprite = sprite;
  cursor->x = x;
  cursor->y = y;
  cursor->scale = scale;

  return cursor;
}

void destroy_cursor(cursor_t *cursor) {
  if (cursor == NULL) return;
  free(cursor);
}

void move_cursor(cursor_t *cursor, int dx, int dy) {
  if (cursor == NULL) return;
  cursor->x += dx;
  cursor->y -= dy;
  if (cursor->x < 0)   cursor->x = 0;
  if (cursor->x > 799) cursor->x = 799;
  if (cursor->y < 0)   cursor->y = 0;
  if (cursor->y > 599) cursor->y = 599;
}

void draw_cursor(cursor_t *cursor) {
  if (cursor == NULL || cursor->sprite == NULL) return;
  int draw_x = cursor->x - (cursor->sprite->width / cursor->scale) / 2;
  int draw_y = cursor->y - (cursor->sprite->height / cursor->scale) / 2;
  draw_sprite_scaled_down(cursor->sprite, draw_x, draw_y, cursor->scale);
}

void update_cursor_sprite(cursor_t *cursor, sprite_t *new_sprite) {
  if (cursor == NULL) return;
  cursor->sprite = new_sprite;
}
