#include "speedometer.h"
#include "speedometer_pixmap.h"
#include "video-card.h"
#include <math.h>
#include <stdlib.h>

static sprite_t *speedo_sprite = NULL;

void speedometer_init() {
    speedo_sprite = create_sprite((xpm_map_t) speedometer_xpm);
}

void speedometer_destroy() {
    destroy_sprite(speedo_sprite);
    speedo_sprite = NULL;
}

/* Draw sprite skipping only transparent, allowing black */
static void sprite_draw_full(sprite_t *sp, int x, int y) {
    if (sp == NULL || sp->map == NULL) return;
    uint32_t transp = xpm_transparency_color(XPM_8_8_8);
    for (int row = 0; row < sp->height; row++) {
        for (int col = 0; col < sp->width; col++) {
            uint32_t color = sp->map[row * sp->width + col];
            if (color != transp)
                vg_buf_draw_pixel(x + col, y + row, color);
        }
    }
}

static void draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;
    while (1) {
        vg_buf_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

static void draw_circle(int cx, int cy, int r, uint32_t color) {
    for (int dy = -r; dy <= r; dy++) {
        for (int dx = -r; dx <= r; dx++) {
            if (dx*dx + dy*dy <= r*r)
                vg_buf_draw_pixel(cx + dx, cy + dy, color);
        }
    }
}

void speedometer_draw(float speed) {
    /* 1. Draw the dial background (no needle, transparent bg) */
    if (speedo_sprite != NULL)
        sprite_draw_full(speedo_sprite, SPEEDO_X, SPEEDO_Y);

    /* 2. Calculate needle angle
       Speed 0   -> -220 deg (full left)
       Speed max ->   40 deg (full right) */
    float t = fabs(speed) / CAR_MAX_SPEED;
    if (t > 1.0f) t = 1.0f;
    if (t < 0.0f) t = 0.0f;

    float angle_deg = NEEDLE_ANGLE_MIN + t * (NEEDLE_ANGLE_MAX - NEEDLE_ANGLE_MIN);
    float angle_rad = angle_deg * M_PI / 180.0f;

    int px = SPEEDO_X + NEEDLE_CX;
    int py = SPEEDO_Y + NEEDLE_CY;

    int ex = px + (int)(NEEDLE_LEN * cos(angle_rad));
    int ey = py + (int)(NEEDLE_LEN * sin(angle_rad));

    /* Draw thick red needle (5 pixels wide) */
    draw_line(px, py, ex, ey, 0xFF0000);
    draw_line(px+1, py, ex+1, ey, 0xFF0000);
    draw_line(px-1, py, ex-1, ey, 0xFF0000);
    draw_line(px, py+1, ex, ey+1, 0xFF0000);
    draw_line(px, py-1, ex, ey-1, 0xFF0000);

    /* Draw pivot circle at base of needle */
    draw_circle(px, py, 4, 0xCCCCCC);
}

