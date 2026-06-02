#include <lcom/lcf.h>
#include "video-card.h"
#include <stdlib.h>
#include <string.h>


/* Static global variables */
static char *video_mem;          /* frame-buffer VM address */
static uint8_t *back_buf = NULL; /* secondary frame-buffer */

/* Other variables that might be used */
static vbe_mode_info_t vmi;      /* VBE mode info */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned h_res;
static unsigned v_res;
static size_t vram_size;

int set_video_mode(uint16_t mode) {
    reg86_t r86;
    
    /* Specify the appropriate register values */
    
    memset(&r86, 0, sizeof(r86)); /* zero the structure */

    r86.intno = 0x10;             /* BIOS video services */
    r86.ah = 0x4F;                /* Set Video Mode - standard BIOS function */
    r86.al = 0x02;
    r86.bx = mode | BIT(14);                    /* 80x25 text mode */
    
    /* Make the BIOS call */

    if (sys_int86(&r86) != OK) {
        printf("\tsys_int86() failed \n");
        return 1;
    }

    return 0;
}

int map_video_memory(uint16_t mode) {
 if (vbe_get_mode_info(mode, &vmi)) return 1;

  h_res = vmi.XResolution;
  v_res = vmi.YResolution;
  bytes_per_pixel = (vmi.BitsPerPixel + 7) / 8;

  vram_size = h_res * v_res * bytes_per_pixel;
  unsigned vram_base = vmi.PhysBasePtr;

  struct minix_mem_range mr;
  mr.mr_base = (phys_bytes) vram_base;
  mr.mr_limit = mr.mr_base + vram_size;

  int r;
  if (OK != (r = sys_privctl(SELF, SYS_PRIV_ADD_MEM, &mr))) {
    printf("sys_privctl (ADD_MEM) failed: %d\n", r);
    return 1;
  }

  video_mem = vm_map_phys(SELF, (void *)mr.mr_base, vram_size);
  if (video_mem == MAP_FAILED) {
    printf("couldn't map video memory\n");
    return 1;
  }

  set_video_mode(mode);
  return 0;
}
    
int vg_draw_pixel(uint16_t x, uint16_t y, uint32_t color) {
  if (x >= h_res || y >= v_res) return 0; // Basic bounds checking

  // Calculate the pointer to the specific pixel
  char *pixel_ptr = video_mem + (y * h_res + x) * bytes_per_pixel;

  // Copy only the relevant number of bytes
  memcpy(pixel_ptr, &color, bytes_per_pixel);

  return 0;
}

int vg_draw_line(uint16_t x, uint16_t y, uint16_t len, uint32_t color) {
    for (uint16_t i = 0; i < len; i++) {
        if (vg_draw_pixel(x + i, y, color) != 0) return 0;
    }

    return 0;
}

int vg_draw_rect(uint16_t x, uint16_t y, uint16_t width,
                      uint16_t height, uint32_t color) {
    for (uint16_t j = 0; j < height; j++) {
        if (vg_draw_line(x, y + j, width, color) != 0) return 0;
    }

    return 0;
}

int vg_draw_xpm(xpm_map_t xpm, uint16_t x, uint16_t y) {
  xpm_image_t img;
  uint8_t *pixmap = xpm_load(xpm, XPM_INDEXED, &img);
  if (pixmap == NULL) return 1;

  for (uint16_t row = 0; row < img.height; row++) {
    for (uint16_t col = 0; col < img.width; col++) {
      uint32_t color = pixmap[row * img.width + col];
      vg_draw_pixel(x + col, y + row, color);
    }
  }
  return 0;
}

int vg_init_double_buffer(uint16_t width, uint16_t height, uint8_t bpp) {
  if (video_mem == NULL) return 1;
  if (width != h_res || height != v_res || bpp != bytes_per_pixel) return 1;

  back_buf = (uint8_t *) malloc(vram_size);
  if (back_buf == NULL) return 1;

  memset(back_buf, 0, vram_size);
  return 0;
}

void vg_buf_clear(void) {
  if (back_buf) memset(back_buf, 0, vram_size);
}

void vg_buf_draw_pixel(int x, int y, uint32_t color) {
  if (!back_buf) return;
  if (x < 0 || x >= (int) h_res || y < 0 || y >= (int) v_res) return;

  size_t offset = ((size_t) y * h_res + x) * bytes_per_pixel;

  if (bytes_per_pixel == 3) {
    back_buf[offset + 0] = color & 0xFF;
    back_buf[offset + 1] = (color >> 8) & 0xFF;
    back_buf[offset + 2] = (color >> 16) & 0xFF;
  }
  else if (bytes_per_pixel == 2) {
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    uint16_t c = ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);

    back_buf[offset + 0] = c & 0xFF;
    back_buf[offset + 1] = (c >> 8) & 0xFF;
  }
}

void vg_buf_draw_rect(int x, int y, int w, int h, uint32_t color) {
  for (int row = 0; row < h; row++) {
    for (int col = 0; col < w; col++) {
      vg_buf_draw_pixel(x + col, y + row, color);
    }
  }
}

void vg_buf_swap(void) {
  if (back_buf && video_mem) {
    memcpy(video_mem, back_buf, vram_size);
  }
}

void vg_buf_desaturate(void) {
  if (!back_buf) return;

  uint32_t total_pixels = h_res * v_res;
  for (uint32_t i = 0; i < total_pixels; i++) {
    size_t off = (size_t) i * bytes_per_pixel;

    if (bytes_per_pixel == 3) {
      uint8_t b = back_buf[off + 0];
      uint8_t g = back_buf[off + 1];
      uint8_t r = back_buf[off + 2];
      uint8_t grey = (uint8_t)(0.299f * r + 0.587f * g + 0.114f * b);

      back_buf[off + 0] = grey;
      back_buf[off + 1] = grey;
      back_buf[off + 2] = grey;
    }
  }
}

void vg_free_double_buffer(void) {
  free(back_buf);
  back_buf = NULL;
}
