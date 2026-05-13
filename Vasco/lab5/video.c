#include <lcom/lcf.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "video.h"

static uint8_t *video_mem;  /* Virtual address to which VRAM is mapped */
static vbe_mode_info_t vmi;      /* VBE mode info */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */

int video_init(uint16_t mode) {

  int r;
  struct minix_mem_range mr;
  unsigned int vram_base;          /* VRAM's physical addresss */
  unsigned int vram_size;          /* VRAM's size, but you can use the frame-buffer size */

  /* 1. Get VBE mode info */
  if (vbe_get_mode_info(mode, &vmi) != 0) {
      printf("vg_init: vbe_get_mode_info failed\n");
      return 1;
  }

  /*2. Set vram_base and vram_size*/
  vram_base = vmi.PhysBasePtr;
  vram_size = vmi.XResolution * vmi.YResolution * (vmi.BitsPerPixel / 8);

  /* 3. Set other static global variables */
  bytes_per_pixel = vmi.BitsPerPixel / 8;

  /* Grant memory mapping permissions */
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;  

  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr)))
   panic("sys_privctl (ADD_MEM) failed: %d\n", r);

  /* Map memory */
  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);

  if(video_mem == MAP_FAILED){
   panic("couldn't map video memory");
  }
  return 0;
}

int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color){
  if (x >= vmi.XResolution || y >= vmi.YResolution) return 1;

  uint8_t *pixel = video_mem + (y * vmi.XResolution + x) * bytes_per_pixel;

  memcpy(pixel, &color , bytes_per_pixel);

  return 0;
}

int (vg_draw_hline)(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
  for (uint16_t i = 0; i < len; i++){
    if (vg_draw_pixel(x + i, y, color) != 0) return 1;
  }
  return 0;
}

int (vg_draw_rectangle)(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color){
  for (uint16_t i = 0; i < height ; i++){
    if (vg_draw_hline(x , y + i, width, color ) != 0) return 1;
  }
  return 0;
}
