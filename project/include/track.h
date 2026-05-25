#ifndef _TRACK_H_
#define _TRACK_H_

#include "sprite.h"

/** Surface types for speed calculation */
typedef enum {
    SURFACE_ROAD,    /* asphalt - full speed */
    SURFACE_SLOW,    /* sand/curbs/markings - reduced speed */
    SURFACE_BLOCKED  /* grass - can't drive */
} surface_t;

/**
 * @brief Returns the surface type at a given position.
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return Surface type at that pixel.
 */
surface_t track_get_surface(int x, int y);

/**
 * @brief Returns the slowest surface under the car's bounding box.
 * @param x Top-left X of the car.
 * @param y Top-left Y of the car.
 * @param width Width of the car sprite.
 * @param height Height of the car sprite.
 * @return The worst (slowest) surface type found.
 */
surface_t track_car_surface(int x, int y, int width, int height);

/**
 * @brief Initializes the track by loading the XPM and building the collision map.
 * @param xpm XPM map of the track.
 * @return 0 on success, non-zero otherwise.
 */
int track_init(xpm_map_t track_xpm);

/**
 * @brief Draws the track on the back buffer.
 */
void track_draw(void);

/**
 * @brief Checks if a single pixel position is on the road (driveable surface).
 * @param x X coordinate.
 * @param y Y coordinate.
 * @return true if on road, false otherwise.
 */
bool track_is_on_road(int x, int y);

/**
 * @brief Checks if a car's bounding box is entirely on the road.
 * @param x Top-left X of the car.
 * @param y Top-left Y of the car.
 * @param width Width of the car sprite.
 * @param height Height of the car sprite.
 * @return true if all corners are on road, false otherwise.
 */
bool track_car_on_road(int x, int y, int width, int height);

/**
 * @brief Frees memory used by the track.
 */
void track_free(void);

#endif /* _TRACK_H_ */
