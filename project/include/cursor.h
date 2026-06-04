#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "sprite.h"

/**
 * @brief Draws a sprite as the mouse cursor, centered on the given position.
 *
 * The sprite is scaled down by the given factor and drawn so that its
 * centre coincides with (cursor_x, cursor_y).  Should be called as the
 * very last draw operation each frame so the cursor is always on top.
 *
 * @param sp       Sprite to use as the cursor image.
 * @param cursor_x X position of the mouse cursor.
 * @param cursor_y Y position of the mouse cursor.
 * @param scale    Downscale factor (e.g. 2 = half size).
 */
void draw_cursor(sprite_t *sp, int cursor_x, int cursor_y, int scale);

#endif /* _CURSOR_H_ */
