#include <lcom/lcf.h>
#include "video-card.h"


/* Static global variables */
static char *video_mem;          /* frame-buffer VM address */

/* Other variables that might be used */
static vbe_mode_info_t vmi;      /* VBE mode info */
static unsigned bytes_per_pixel; /* Number of VRAM bytes per pixel */
static unsigned h_res;
static unsigned v_res;

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

  unsigned vram_size = h_res * v_res * bytes_per_pixel;
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
