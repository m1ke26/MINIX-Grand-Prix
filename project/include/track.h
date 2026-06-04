#ifndef _TRACK_H_
#define _TRACK_H_

#include <lcom/lcf.h>

/**
 * @brief Structure that holds the track data (visual map and collision map).
 */
typedef struct
{
    xpm_image_t info_trackmap; /**< @brief Info (width, height) of the visual map */
    uint8_t *pix_trackmap;     /**< @brief Pixel data of the visual map */

    xpm_image_t info_collisionmap; /**< @brief Info (width, height) of the collision map */
    uint8_t *pix_collisionmap;     /**< @brief Pixel data of the collision map */

} track_t;


/**
 * @brief Creates a track by loading the visual and collision XPM maps.
 * @param track XPM data for the visual map.
 * @param collision XPM data for the collision map.
 * @return Pointer to the created track, or NULL on failure.
 */
track_t *create_track(xpm_map_t track, xpm_map_t collision);

/**
 * @brief Draws the visible portion of the track on the screen.
 * @details For each screen pixel (x,y), fetches the corresponding pixel at (cam_x + x, cam_y + y) from the visual map.
 * @param track Pointer to the track.
 * @param cam_x X coordinate of the camera (top-left corner of the visible area).
 * @param cam_y Y coordinate of the camera (top-left corner of the visible area).
 */
void draw_track(track_t *track, int cam_x, int cam_y);

/**
 * @brief Checks the terrain type at a given position on the collision map.
 * @param track Pointer to the track.
 * @param car_x X coordinate to check.
 * @param car_y Y coordinate to check.
 * @return 0 if road (black), 1 if blocked (red), 2 if slow (yellow).
 */
int collision_track(track_t *track, int car_x, int car_y);

/**
 * @brief Destroys the track and frees allocated memory.
 * @param track Pointer to the track to be destroyed.
 */
void destroy_track(track_t *track);

#endif

