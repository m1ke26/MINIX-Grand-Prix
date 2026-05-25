#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <lcom/lcf.h>

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

typedef struct {
    int x, y;           /* top-left corner of the viewport in world coordinates */
    int map_width;      /* total map width */
    int map_height;     /* total map height */
} camera_t;

/**
 * @brief Initializes the camera with the map dimensions.
 * @param map_w Map width in pixels.
 * @param map_h Map height in pixels.
 */
void camera_init(int map_w, int map_h);

/**
 * @brief Centers the camera on a target position (e.g. the car).
 * @param target_x X coordinate of the target (world space).
 * @param target_y Y coordinate of the target (world space).
 */
void camera_follow(int target_x, int target_y);

/**
 * @brief Returns the camera X offset (for drawing).
 */
int camera_get_x(void);

/**
 * @brief Returns the camera Y offset (for drawing).
 */
int camera_get_y(void);

#endif /* _CAMERA_H_ */
