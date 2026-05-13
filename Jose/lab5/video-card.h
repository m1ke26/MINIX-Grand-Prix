#include <lcom/lcf.h>

#include <lcom/lab5.h>

#include <stdint.h>
#include <stdio.h>

int set_video_mode(uint16_t mode);
int map_video_memory(uint16_t mode);
int vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                      uint16_t height, uint32_t color);
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

