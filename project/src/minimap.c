#include "minimap.h"
#include "minimap1_pixmap.h"
#include "minimap2_pixmap.h"
#include "minimap3_pixmap.h"
#include "video-card.h"
#include <stdlib.h>

static sprite_t *minimap_sprite = NULL;
static int map_w = 0;
static int map_h = 0;

void minimap_init(int track_index, track_t *track) {
    minimap_destroy();

    if (track != NULL) {
        map_w = track->info_trackmap.width;
        map_h = track->info_trackmap.height;
    }

    switch(track_index) {
        case 0: minimap_sprite = create_sprite((xpm_map_t) minimap1_xpm); break;
        case 1: minimap_sprite = create_sprite((xpm_map_t) minimap2_xpm); break;
        case 2: minimap_sprite = create_sprite((xpm_map_t) minimap3_xpm); break;
        default: minimap_sprite = create_sprite((xpm_map_t) minimap1_xpm); break;
    }
}

void minimap_destroy() {
    if (minimap_sprite != NULL) {
        destroy_sprite(minimap_sprite);
        minimap_sprite = NULL;
    }
}

static void draw_circle(int cx, int cy, int r, uint32_t color) {
    for (int dy = -r; dy <= r; dy++)
        for (int dx = -r; dx <= r; dx++)
            if (dx*dx + dy*dy <= r*r)
                vg_buf_draw_pixel(cx + dx, cy + dy, color);
}

static void sprite_draw_full(sprite_t *sp, int x, int y) {
    if (sp == NULL || sp->map == NULL) return;
    uint32_t transp = xpm_transparency_color(XPM_8_8_8);
    for (int row = 0; row < sp->height; row++)
        for (int col = 0; col < sp->width; col++) {
            uint32_t color = sp->map[row * sp->width + col];
            if (color != transp)
                vg_buf_draw_pixel(x + col, y + row, color);
        }
}

void minimap_draw(int car_x, int car_y) {
    if (minimap_sprite == NULL) return;

    sprite_draw_full(minimap_sprite, MINIMAP_X, MINIMAP_Y);

    if (map_w > 0 && map_h > 0) {
        int offset_y = 4;
        int dot_x = MINIMAP_X + (car_x * MINIMAP_W / map_w);
        int dot_y = MINIMAP_Y + (car_y * MINIMAP_H / map_h) + offset_y;
        draw_circle(dot_x, dot_y, 2, 0xFF0000);
    }
}

