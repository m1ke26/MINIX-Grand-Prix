#include "track.h"
#include <stdlib.h>
#include "video-card.h"

track_t *create_track(xpm_map_t track, xpm_map_t collision)
{
    track_t *new_track = (track_t *)malloc(sizeof(track_t)); /*reserva memória para a struct track */
    if (new_track == NULL)                                   /* se falhar, sai*/
        return NULL;

    new_track->pix_trackmap = xpm_load(track, XPM_8_8_8, &new_track->info_trackmap);
    /* carrega o map visual */
    new_track->pix_collisionmap = xpm_load(collision, XPM_8_8_8, &new_track->info_collisionmap);
    /* carrega o mapa de colisão*/
    return new_track;
}

void draw_track(track_t *track, int cam_x, int cam_y)
{
    for (int y = 0; y < 600; y++)
    {
        for (int x = 0; x < 800; x++)
        {
            int map_x = cam_x + x;
            /*
            cam_x e onde a camera esta no mapa, vai de 0 a 2156
            x e o pixel do ecra, vai de 0 a 799, 800 pixeis, do 800x600 ecra do minix
            cam_x esta limitado a 2156 para q que o pixel chegar ao 799 e somar ao 2156
            n ultrapassa o limite do mapa q tem comprimento maximo de 2956px
            map_x <= 2156
            */
            int map_y = cam_y + y;
            /*
            cam_y e onde a camera esta no mapa, vai de 0 a 1617
            y e o pixel do ecra, vai de 0 a 599, 600 pixeis, do 800x600 ecra do minix
            cam_y esta limitado a 1617 para q que o pixel chegar ao 599 e somar ao 1617
            n ultrapassa o limite do mapa q tem altura maxima de 2217px
            map_y <= 2217
            */
            int pos = (map_y * track->info_trackmap.width + map_x) * 3;                                                                 /*calcula onde esta o 1º byte do pixel no pix_collisionmap, é vezes 3 pq cada pixel ocupa 3 bytes*/
            uint32_t color = ((track->pix_trackmap[pos]) + (track->pix_trackmap[pos + 1] << 8) + (track->pix_trackmap[pos + 2] << 16)); /*junta os 3 bytes separados numa unica cor em formato 0X00RRGGBB*/
            /*
            pos = azul (B), fica nos bits 0-7 (sem shift)
            pos + 1 = verde (G), fica nos bits 8-15 (<< 8)
            post + 2 = vermelho (R), fica nos bist 16-23 (<< 16)
            */
            vg_buf_draw_pixel(x, y, color); /* tirada do double_buffer.c do zé, pinta um pixel na pos (x,y) com a cor dada*/
        }
    }
}

terrain_type_t collision_track(track_t *track, int car_x, int car_y)
{
    int pos = (car_y * track->info_collisionmap.width + car_x) * 3;
    uint32_t color = ((track->pix_collisionmap[pos]) + (track->pix_collisionmap[pos + 1] << 8) + (track->pix_collisionmap[pos + 2] << 16));

    if (color == TERRAIN_BLOCKED) /*vermelho*/
    {
        return TERRAIN_BLOCKED;
    }
    if (color == TERRAIN_ROAD) /*preto*/
    {
        return TERRAIN_ROAD;
    }
    if (color == TERRAIN_SLOW) /*amarelo*/
    {
        return TERRAIN_SLOW;
    }
    if (color == START) /*verde*/
    {
        return START;
    }
    if (color == CHECKPOINT_1) /*ciano*/
    {
        return CHECKPOINT_1;
    }
    if (color == CHECKPOINT_2) /*magenta*/
    {
        return CHECKPOINT_2;
    }
    if (color == CHECKPOINT_3) /*azul*/
    {
        return CHECKPOINT_3;
    }
    return TERRAIN_ROAD;
}

static bool is_race_checkpoint(terrain_type_t t) {
    return t == START || t == CHECKPOINT_1 || t == CHECKPOINT_2 || t == CHECKPOINT_3;
}

bool track_find_start_spawn(track_t *track, int car_w, int car_h, int *spawn_x, int *spawn_y) {
    if (track == NULL || spawn_x == NULL || spawn_y == NULL || car_w <= 0 || car_h <= 0)
        return false;

    int w = track->info_collisionmap.width;
    int h = track->info_collisionmap.height;
    long long sum_x = 0;
    long long sum_y = 0;
    long long count = 0;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            int pos = (y * w + x) * 3;
            uint32_t color = (uint32_t)track->pix_collisionmap[pos]
                + ((uint32_t)track->pix_collisionmap[pos + 1] << 8)
                + ((uint32_t)track->pix_collisionmap[pos + 2] << 16);

            if (color == (uint32_t)START) {
                sum_x += x;
                sum_y += y;
                count++;
            }
        }
    }

    if (count == 0)
        return false;

    int cx = (int)(sum_x / count);
    int cy = (int)(sum_y / count);
    int sx = cx - car_w / 2;
    int sy = cy - car_h / 2;

    if (sx < 0) sx = 0;
    if (sy < 0) sy = 0;
    if (sx > w - car_w) sx = w - car_w;
    if (sy > h - car_h) sy = h - car_h;

    *spawn_x = sx;
    *spawn_y = sy;
    return true;
}

terrain_type_t track_car_checkpoint(track_t *track, int car_x, int car_y, int car_w, int car_h) {
    if (track == NULL || car_w <= 0 || car_h <= 0)
        return TERRAIN_ROAD;

    //4 corners + center of the car
    int samples[5][2] = {
        {car_x + car_w / 2, car_y + car_h / 2},
        {car_x, car_y},
        {car_x + car_w - 1, car_y},
        {car_x, car_y + car_h - 1},
        {car_x + car_w - 1, car_y + car_h - 1},
    };

    for (int i = 0; i < 5; i++) {
        terrain_type_t t = collision_track(track, samples[i][0], samples[i][1]);
        if (is_race_checkpoint(t))
            return t;
    }
    return TERRAIN_ROAD;
}

void destroy_track(track_t *track)
{
    free(track); /* liberta a memoria alocada pelo create_track */
}
