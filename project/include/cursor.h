#ifndef _CURSOR_H_
#define _CURSOR_H_

#include "sprite.h"

typedef struct {
  sprite_t *sprite;
  int x, y;
  int scale;
} cursor_t;

/**
 * @brief Initializes the cursor with the given sprite, position, and scale.
 * @param sprite Pointer to the sprite to use for the cursor.
 * @param x Initial x-coordinate of the cursor.
 * @param y Initial y-coordinate of the cursor.
 * @param scale Initial scale of the cursor.
 * @return Pointer to the initialized cursor structure.
 */
cursor_t* create_cursor(sprite_t *sprite, int x, int y, int scale);

/**
 * @brief Destroys the cursor and frees any allocated resources.
 * @param cursor Pointer to the cursor to be destroyed.
 */
void destroy_cursor(cursor_t *cursor);

/**
 * @brief Moves the cursor by the specified deltas in the x and y directions.
 * @param cursor Pointer to the cursor to be moved.
 * @param dx Change in x-coordinate.
 * @param dy Change in y-coordinate.
 */
void move_cursor(cursor_t *cursor, int dx, int dy);

/**
 * @brief Draws the cursor on the screen at its current position, applying scaling if necessary.
 *
 * @param cursor Pointer to the cursor structure containing the sprite and its properties.
 */
void draw_cursor(cursor_t *cursor);

/**
 * @brief Updates the sprite used by the cursor without destroying the old one.
 * @param cursor Pointer to the cursor.
 * @param new_sprite Pointer to the new sprite to use.
 */
void update_cursor_sprite(cursor_t *cursor, sprite_t *new_sprite);

#endif /* _CURSOR_H_ */
