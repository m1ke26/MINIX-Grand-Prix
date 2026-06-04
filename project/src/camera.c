#include "camera.h"
#include <stdlib.h>

camera_t *create_camera(int map_width, int map_height)
{
    camera_t *new_camera = (camera_t *)malloc(sizeof(camera_t)); /*reserva memória para a struct camera */
    if (new_camera == NULL)                                      /* se falhar, sai*/
        return NULL;

    new_camera->cam_x = 0;
    new_camera->cam_y = 0;

    new_camera->max_x = map_width - 800;  /*2156*/
    new_camera->max_y = map_height - 600; /*2217*/

    return new_camera;
}

void follow_camera(camera_t *camera, int car_x, int car_y)
{
    camera->cam_x = car_x - 400;
    camera->cam_y = car_y - 300;

    if (camera->cam_x < 0)
    {
        camera->cam_x = 0;
    }

    if (camera->cam_y < 0)
    {
        camera->cam_y = 0;
    }

    if (camera->cam_x > camera->max_x)
    {
        camera->cam_x = camera->max_x;
    }

    if (camera->cam_y > camera->max_y)
    {
        camera->cam_y = camera->max_y;
    }
}

void destroy_camera(camera_t *camera)
{
    free(camera); /* liberta a memoria alocada pelo create_camera */
}
