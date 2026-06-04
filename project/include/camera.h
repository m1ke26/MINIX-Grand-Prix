#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <lcom/lcf.h>

/**
 * @brief Structure that holds the camera state for viewport scrolling.
 */
typedef struct
{
    int cam_x; /**< @brief Current X position of the camera (top-left corner) */
    int max_x; /**< @brief Maximum X position (map_width - screen_width) */
    int cam_y; /**< @brief Current Y position of the camera (top-left corner) */
    int max_y; /**< @brief Maximum Y position (map_height - screen_height) */

} camera_t;

/**
 * @brief Creates a camera with calculated limits based on the map size.
 * @param map_width Width of the map in pixels.
 * @param map_height Height of the map in pixels.
 * @return Pointer to the created camera, or NULL on failure.
 */
camera_t *create_camera(int map_width, int map_height);

/**
 * @brief Updates the camera position to follow the car, keeping it centered on screen.
 * @details Centers the camera on the car position and clamps to map boundaries.
 * @param camera Pointer to the camera.
 * @param car_x X coordinate of the car.
 * @param car_y Y coordinate of the car.
 */
void follow_camera(camera_t *camera, int car_x, int car_y);

/**
 * @brief Destroys the camera and frees allocated memory.
 * @param camera Pointer to the camera to be destroyed.
 */
void destroy_camera(camera_t *camera);

#endif

