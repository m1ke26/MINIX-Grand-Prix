#include "sprite.h"
#include "colors.h"
#include "video-card.h"
#include <stdlib.h>

sprite_t* create_sprite(xpm_map_t xpm) {
  sprite_t *sp = (sprite_t *) malloc(sizeof(sprite_t));
  if (sp == NULL) return NULL;

  xpm_image_t img;
  /* XPM_8_8_8 gives 3 bytes per pixel: R, G, B */
  uint8_t *map = xpm_load(xpm, XPM_8_8_8, &img);
  if (map == NULL) {
    free(sp);
    return NULL;
  }

  sp->width  = img.width;
  sp->height = img.height;

  /* Convert RGB (3 bytes) to packed uint32_t (0x00RRGGBB) */
  uint32_t npixels = img.width * img.height;
  sp->map = (uint32_t *) malloc(npixels * sizeof(uint32_t));
  if (sp->map == NULL) {
    free(map);
    free(sp);
    return NULL;
  }

  for (uint32_t i = 0; i < npixels; i++) {
    uint8_t r = map[i * 3 + 2];
    uint8_t g = map[i * 3 + 1];
    uint8_t b = map[i * 3 + 0];
    sp->map[i] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
  }

  free(map);
  return sp;
}

void destroy_sprite(sprite_t *sp) {
  if (sp == NULL) return;
  if (sp->map) free(sp->map);
  free(sp);
}

void sprite_draw(sprite_t *sp, int x, int y) {
  if (sp == NULL || sp->map == NULL) return;

  uint32_t transp = xpm_transparency_color(XPM_8_8_8);

  for (int row = 0; row < sp->height; row++) {
    for (int col = 0; col < sp->width; col++) {
      uint32_t color = sp->map[row * sp->width + col];
      if (color != transp && color != COLOR_BLACK)
        vg_buf_draw_pixel(x + col, y + row, color);
    }
  }
}

void draw_sprite_scaled_down(sprite_t *s, int x, int y, int scale) {
  if (s == NULL) return;
  uint32_t transp = xpm_transparency_color(XPM_8_8_8);
  if (scale <= 0) return;

  int w = s->width  / scale;
  int h = s->height / scale;
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      uint32_t pixel = s->map[(row * scale) * s->width + (col * scale)];
      if (pixel == transp || pixel == COLOR_BLACK) continue;
      vg_buf_draw_pixel(x + col, y + row, pixel);
    }
  }
}

void draw_sprite_scaled_up(sprite_t *s, int x, int y, int scale) {
  if (s == NULL || s->map == NULL) return;
  if (scale <= 0) return;

  uint32_t transp = xpm_transparency_color(XPM_8_8_8);

  int w = s->width  * scale;
  int h = s->height * scale;

  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      int src_row = row / scale;
      int src_col = col / scale;
      uint32_t pixel = s->map[src_row * s->width + src_col];

      if (pixel == transp || pixel == COLOR_BLACK) continue;

      vg_buf_draw_pixel(x + col, y + row, pixel);
    }
  }
}

