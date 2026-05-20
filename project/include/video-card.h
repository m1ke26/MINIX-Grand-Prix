#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>

int set_video_mode(uint16_t mode);
int map_video_memory(uint16_t mode);
int vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color);
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color);
int vg_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                      uint16_t height, uint32_t color);
int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y);

/* ---- Double buffering ---- */

/** Must be called once after map_video_memory(). */
int  vg_init_double_buffer(uint16_t width, uint16_t height, uint8_t bytes_per_pixel);

/** Draw pixel to back buffer. */
void vg_buf_draw_pixel(int x, int y, uint32_t color);

/** Draw filled rect to back buffer. */
void vg_buf_draw_rect(int x, int y, int w, int h, uint32_t color);

/** Clear back buffer to black. */
void vg_buf_clear(void);

/** Copy back buffer to framebuffer (one memcpy). */
void vg_buf_swap(void);

/** Free back buffer memory. */
void vg_free_double_buffer(void);
