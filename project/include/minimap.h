#ifndef _MINIMAP_H_
#define _MINIMAP_H_

#include <lcom/lcf.h>
#include "sprite.h"
#include "track.h"

#define MINIMAP_X 610
#define MINIMAP_Y 10
#define MINIMAP_W 180
#define MINIMAP_H 135

/**
 * @brief Initializes the minimap for the given track index and track data.
 * @param track_index The index of the track to load minimap data for.
 * @param track Pointer to the track data to use for generating the minimap.
 */
void minimap_init(int track_index, track_t *track);

/**
 * @brief Draws the minimap on the screen, showing the car's position.
 * @param car_x The x-coordinate of the car on the track.
 * @param car_y The y-coordinate of the car on the track.
 */
void minimap_draw(int car_x, int car_y);

/**
 * @brief Destroys the minimap and frees any allocated resources.
 */
void minimap_destroy();

#endif /* _MINIMAP_H_ */
