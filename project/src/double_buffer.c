#include "video-card.h"
#include <lcom/lcf.h>
#include <stdlib.h>
#include <string.h>

static uint8_t  *video_mem  = NULL;
static uint16_t  scr_width  = 0;
static uint16_t  scr_height = 0;
static uint8_t   scr_bpp    = 0;
static uint8_t  *back_buf   = NULL;
static size_t    buf_size   = 0;

int vg_init_double_buffer(uint16_t width, uint16_t height, uint8_t bytes_per_pixel) {
  scr_width  = width;
  scr_height = height;
  scr_bpp    = bytes_per_pixel;

  /* Map the framebuffer ourselves using VBE info */
  vbe_mode_info_t vmi;
  if (vbe_get_mode_info(0x115, &vmi) != 0) return 1;

  unsigned int vram_base = vmi.PhysBasePtr;
  unsigned int vram_size = width * height * bytes_per_pixel;

  struct minix_mem_range mr;
  mr.mr_base  = vram_base;
  mr.mr_limit = vram_base + vram_size;
  if (sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr) != OK) return 1;

  video_mem = (uint8_t *) vm_map_phys(SELF, (void *)(uintptr_t)vram_base, vram_size);
  if (video_mem == MAP_FAILED) { video_mem = NULL; return 1; }

  buf_size = (size_t)width * height * bytes_per_pixel;
  back_buf = (uint8_t *) malloc(buf_size);
  if (back_buf == NULL) return 1;

  memset(back_buf, 0, buf_size);
  return 0;
}

void vg_buf_clear(void) {
  if (back_buf) memset(back_buf, 0, buf_size);
}

void vg_buf_draw_pixel(int x, int y, uint32_t color) {
  if (!back_buf) return;
  if (x < 0 || x >= scr_width || y < 0 || y >= scr_height) return;

  size_t offset = ((size_t)y * scr_width + x) * scr_bpp;

  if (scr_bpp == 3) {
    back_buf[offset + 0] =  color        & 0xFF;
    back_buf[offset + 1] = (color >>  8) & 0xFF;
    back_buf[offset + 2] = (color >> 16) & 0xFF;
  } else if (scr_bpp == 2) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >>  8) & 0xFF;
    uint8_t b =  color        & 0xFF;
    uint16_t c = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
    back_buf[offset + 0] =  c       & 0xFF;
    back_buf[offset + 1] = (c >> 8) & 0xFF;
  }
}

void vg_buf_draw_rect(int x, int y, int w, int h, uint32_t color) {
  for (int row = 0; row < h; row++)
    for (int col = 0; col < w; col++)
      vg_buf_draw_pixel(x + col, y + row, color);
}

void vg_buf_swap(void) {
  if (back_buf && video_mem)
    memcpy(video_mem, back_buf, buf_size);
}

void vg_free_double_buffer(void) {
  free(back_buf);
  back_buf = NULL;
}
