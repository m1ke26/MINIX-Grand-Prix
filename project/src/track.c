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

int collision_track(track_t *track, int car_x, int car_y)
{
    int pos = (car_y * track->info_collisionmap.width + car_x) * 3;
    uint32_t color = ((track->pix_collisionmap[pos]) + (track->pix_collisionmap[pos + 1] << 8) + (track->pix_collisionmap[pos + 2] << 16));

    if (color == 0x000000) /*preto*/
    {
        return 0;
    }
    if (color == 0xff0000) /*vermelho*/
    {
        return 1;
    }
    if (color == 0xffff00) /*amarelo*/
    {
        return 2;
    }
    if (color == 0x00ff00) /*verde, checkpoint 1*/
    {
        return 3;
    }
    if (color == 0x0000ff) /*azul, checkpoint 2*/
    {
        return 4;
    }
    if (color == 0xff00ff) /*magenta, checkpoint 3*/
    {
        return 5;
    }
    if (color == 0x00ffff) /*ciano, checkpoint 4*/
    {
        return 6;
    }
    return 0;
}

void destroy_track(track_t *track)
{
    free(track); /* liberta a memoria alocada pelo create_track */
}
