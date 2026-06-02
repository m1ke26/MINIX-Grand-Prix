#include "track.h"
#include "camera.h"
#include "colors.h"
#include "video-card.h"
#include <stdlib.h>

static sprite_t *track_sprite = NULL;
static uint32_t *collision_map = NULL;
static uint16_t track_width = 0;
static uint16_t track_height = 0;

/* Classify a pixel color into a surface type */
static surface_t classify_color(uint32_t color) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;

    /* Grass: olive-green with low blue. (g-b) is the key: grass has ~85+, edge pixels have <15 */
    if (g > r && g > b && (g - b) > 55)
        return SURFACE_BLOCKED;

    /* Sand/beige: warm tones, R and G high, B much lower */
    if (r > 120 && g > 90 && b < g && (r - b) > 40 && (g - b) > 30)
        return SURFACE_SLOW;

    /* Everything else (asphalt, curbs, markings, red/white) = road */
    return SURFACE_ROAD;
}

static bool is_road_color(uint32_t color) {
    return classify_color(color) != SURFACE_BLOCKED;
}

int (track_init)(xpm_map_t track_xpm) {
    track_sprite = create_sprite(track_xpm);
    if (track_sprite == NULL) return 1;

    track_width = track_sprite->width;
    track_height = track_sprite->height;

    /* The collision map is just the sprite's pixel data */
    collision_map = track_sprite->map;

    return 0;
}

void track_draw(void) {
    if (track_sprite == NULL || collision_map == NULL) return;

    int cam_x = camera_get_x();
    int cam_y = camera_get_y();

    /* Only draw the visible 800x600 portion of the map */
    for (int row = 0; row < SCREEN_HEIGHT; row++) {
        for (int col = 0; col < SCREEN_WIDTH; col++) {
            int map_x = cam_x + col;
            int map_y = cam_y + row;
            if (map_x >= 0 && map_x < track_width && map_y >= 0 && map_y < track_height) {
                uint32_t color = collision_map[map_y * track_width + map_x];
                if (color != COLOR_BLACK)
                    vg_buf_draw_pixel(col, row, color);
            }
        }
    }
}

bool (track_is_on_road)(int x, int y) {
    if (collision_map == NULL) return true;  /* No track loaded = no collision */
    if (x < 0 || x >= track_width || y < 0 || y >= track_height) return false;

    uint32_t color = collision_map[y * track_width + x];
    return is_road_color(color);
}

bool (track_car_on_road)(int x, int y, int width, int height) {
    /* Shrink collision box to 60% of sprite size (centered) */
    int margin_x = width / 5;
    int margin_y = height / 5;
    int cx = x + margin_x;
    int cy = y + margin_y;
    int cw = width - 2 * margin_x;
    int ch = height - 2 * margin_y;

    if (!track_is_on_road(cx, cy)) return false;
    if (!track_is_on_road(cx + cw - 1, cy)) return false;
    if (!track_is_on_road(cx, cy + ch - 1)) return false;
    if (!track_is_on_road(cx + cw - 1, cy + ch - 1)) return false;
    if (!track_is_on_road(cx + cw / 2, cy)) return false;
    if (!track_is_on_road(cx + cw / 2, cy + ch - 1)) return false;
    if (!track_is_on_road(cx, cy + ch / 2)) return false;
    if (!track_is_on_road(cx + cw - 1, cy + ch / 2)) return false;

    return true;
}

surface_t (track_get_surface)(int x, int y) {
    if (collision_map == NULL) return SURFACE_ROAD;
    if (x < 0 || x >= track_width || y < 0 || y >= track_height) return SURFACE_BLOCKED;

    uint32_t color = collision_map[y * track_width + x];
    return classify_color(color);
}

surface_t (track_car_surface)(int x, int y, int width, int height) {
    /* Shrink collision box to 60% of sprite size (centered) to avoid
       transparent corners hitting grass */
    int margin_x = width / 5;
    int margin_y = height / 5;
    int cx = x + margin_x;
    int cy = y + margin_y;
    int cw = width - 2 * margin_x;
    int ch = height - 2 * margin_y;

    int points[][2] = {
        {cx, cy}, {cx + cw - 1, cy},                       /* top corners */
        {cx, cy + ch - 1}, {cx + cw - 1, cy + ch - 1},     /* bottom corners */
        {cx + cw / 2, cy}, {cx + cw / 2, cy + ch - 1},     /* top/bottom mid */
        {cx, cy + ch / 2}, {cx + cw - 1, cy + ch / 2}       /* left/right mid */
    };

    surface_t worst = SURFACE_ROAD;
    for (int i = 0; i < 8; i++) {
        surface_t s = track_get_surface(points[i][0], points[i][1]);
        if (s > worst) worst = s;  /* BLOCKED > SLOW > ROAD */
    }
    return worst;
}

void track_free(void) {
    if (track_sprite != NULL) {
        destroy_sprite(track_sprite);
        track_sprite = NULL;
    }
    collision_map = NULL;
    track_width = 0;
    track_height = 0;
}
