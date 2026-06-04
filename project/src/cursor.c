#include "cursor.h"

void draw_cursor(sprite_t *sp, int cursor_x, int cursor_y, int scale) {
    if (sp == NULL || scale <= 0) return;

    /* Compute the scaled dimensions so we can centre the sprite */
    int w = sp->width  / scale;
    int h = sp->height / scale;

    int x = cursor_x - w / 2;
    int y = cursor_y - h / 2;

    draw_sprite_scaled_down(sp, x, y, scale);
}
