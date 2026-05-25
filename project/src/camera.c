#include "camera.h"

static camera_t cam = {0, 0, 0, 0};

void camera_init(int map_w, int map_h) {
    cam.x = 0;
    cam.y = 0;
    cam.map_width = map_w;
    cam.map_height = map_h;
}

void camera_follow(int target_x, int target_y) {
    /* Center the camera on the target */
    cam.x = target_x - SCREEN_WIDTH / 2;
    cam.y = target_y - SCREEN_HEIGHT / 2;

    /* Clamp so camera doesn't go outside the map */
    if (cam.x < 0) cam.x = 0;
    if (cam.y < 0) cam.y = 0;
    if (cam.x > cam.map_width - SCREEN_WIDTH) cam.x = cam.map_width - SCREEN_WIDTH;
    if (cam.y > cam.map_height - SCREEN_HEIGHT) cam.y = cam.map_height - SCREEN_HEIGHT;
}

int camera_get_x(void) {
    return cam.x;
}

int camera_get_y(void) {
    return cam.y;
}
