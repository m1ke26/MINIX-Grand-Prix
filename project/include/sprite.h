#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <lcom/lcf.h>

/**
 * @struct sprite_t
 * @brief Structure representing a sprite.
 */
typedef struct {
  uint16_t width, height;    /**< dimensions */
  uint32_t *map;            /**< the pixmap */
} sprite_t;

/**
 * @brief Creates a sprite from a 24/32-bit XPM.
 * @param xpm XPM map.
 * @return Pointer to the created sprite.
 */
sprite_t* create_sprite(xpm_map_t xpm);

/**
 * @brief Destroys a sprite and frees its memory.
 * @param sp Pointer to the sprite to destroy.
 */
void destroy_sprite(sprite_t *sp);

/**
 * @brief Draws a sprite to the frame buffer at a specific position.
 * @param sp Pointer to the sprite.
 * @param x X position on screen.
 * @param y Y position on screen.
 */
void sprite_draw(sprite_t *sp, int x, int y);

/**
 * @brief Draws a sprite scaled down by a given factor.
 * Samples 1 pixel every `scale` pixels from the original sprite.
 * @param s Pointer to the sprite to draw.
 * @param x X coordinate of the top-left corner on screen.
 * @param y Y coordinate of the top-left corner on screen.
 * @param scale Downscale factor (e.g. 10 = draws at 1/10th of original size).
 */

void draw_sprite_scaled_down(sprite_t *s, int x, int y, int scale);

/**
 * @brief Draws a sprite scaled up by a given factor.
 * Samples 1 pixel every `scale` pixels from the original sprite.
 * @param s Pointer to the sprite to draw.
 * @param x X coordinate of the top-left corner on screen.
 * @param y Y coordinate of the top-left corner on screen.
 * @param scale Upscale factor (e.g. 2 = draws at 2 times of original size).
 */

void draw_sprite_scaled_up(sprite_t *s, int x, int y, double scale);

#endif /* _SPRITE_H_ */
