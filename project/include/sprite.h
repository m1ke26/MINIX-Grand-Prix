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

#endif /* _SPRITE_H_ */
