#include "font.h"
#include "colors.h"
#include <stdlib.h>
#include <string.h>
#include "video-card.h"

// Procedural converter that reads directly from the global font_bits array
xpm_map_t create_xpm_font(int index) {
  // 1 header row + 2 color rows + 8 pixel rows = 11 strings total
  char **xpm = (char **)malloc(sizeof(char *) * 11);
  if (xpm == NULL) return NULL;

  xpm[0] = strdup("8 8 2 1");
  xpm[1] = strdup("  c #000000"); // 0 bit = transparent
  xpm[2] = strdup("X c #222222"); // 1 bit = font ink

  // Pull the 8 rows of bits for this specific character index
  uint8_t const *char_rows = font_bits[index];

  for (int r = 0; r < 8; r++) {
    char *row_str = (char *)malloc(9); // 8 pixels + null terminator
    if (row_str == NULL) {
      // In a bulletproof implementation, you'd unwind previous allocations here
      return NULL;
    }
    
    uint8_t row_bits = char_rows[r];
    for (int c = 0; c < 8; c++) {
      // Read bits from Most Significant Bit (left) to Least Significant Bit (right)
      if ((row_bits & (1 << (7 - c))) != 0) {
        row_str[c] = 'X';
      } else {
        row_str[c] = ' ';
      }
    }
    row_str[8] = '\0';
    xpm[3 + r] = row_str;
  }

  return (xpm_map_t)xpm;
}

void free_generated_xpm(xpm_map_t xpm) {
  char **matrix = (char **)xpm;
  if (matrix == NULL) return;
  for (int i = 0; i < 11; i++) {
    free(matrix[i]);
  }
  free(matrix);
}


font_t* font_create() {
  font_t *font = (font_t *)malloc(sizeof(font_t));
  if (font == NULL) return NULL;

  font->tile_size = 8;
  font->number_of_tiles = 95; // Entire ASCII 32-126 block
  font->tiles = (sprite_t **)malloc(sizeof(sprite_t *) * font->number_of_tiles);
  
  if (font->tiles == NULL) {
    free(font);
    return NULL;
  }

  // Generate individual character tiles dynamically using the index
  for (uint32_t i = 0; i < font->number_of_tiles; i++) {
    xpm_map_t generated_map = create_xpm_font(i);
    font->tiles[i] = create_sprite(generated_map);
    free_generated_xpm(generated_map); // Sprite deep-copied it, safe to free raw wrapper
  }

  return font;
}

void font_destroy(font_t *font) {
  if (font == NULL) return;
  for (uint32_t i = 0; i < font->number_of_tiles; i++) {
    if (font->tiles[i]) {
      if (font->tiles[i]->map) free(font->tiles[i]->map);
      free(font->tiles[i]);
    }
  }
  free(font->tiles);
  free(font);
}

void draw_string(font_t *font, const char *str, int x, int y, uint32_t color) {
  if (font == NULL || str == NULL) return;

  uint32_t transp = xpm_transparency_color(XPM_8_8_8);
  int curr_x = x;
  for (size_t i = 0; i < strlen(str); i++) {
    char c = str[i];
    int index = -1;

    if (c >= 32 && c <= 126) index = c - 32;

    if (index >= 0 && (uint32_t)index < font->number_of_tiles) {
      sprite_t *tile = font->tiles[index];
      if (tile == NULL || tile->map == NULL) { curr_x += font->tile_size; continue; }
      for (int row = 0; row < tile->height; row++) {
        for (int col = 0; col < tile->width; col++) {
          uint32_t pixel = tile->map[row * tile->width + col];
          if (pixel != transp && pixel != COLOR_BLACK)
            vg_buf_draw_pixel(curr_x + col, y + row, color);
        }
      }
    }
    curr_x += font->tile_size;
  }
}

void draw_string_scaled(font_t *font, const char *str, int x, int y, int scale, uint32_t color) {
  if (font == NULL || str == NULL || scale < 1) return;

  uint32_t transp = xpm_transparency_color(XPM_8_8_8);
  int curr_x = x;
  for (size_t i = 0; i < strlen(str); i++) {
    char c = str[i];
    int index = -1;

    if (c >= 32 && c <= 126) index = c - 32;

    if (index >= 0 && (uint32_t)index < font->number_of_tiles) {
      sprite_t *tile = font->tiles[index];
      if (tile == NULL || tile->map == NULL) { curr_x += font->tile_size * scale; continue; }

      for (int row = 0; row < tile->height; row++) {
        for (int col = 0; col < tile->width; col++) {
          uint32_t pixel = tile->map[row * tile->width + col];
          if (pixel != transp && pixel != COLOR_BLACK)
            vg_buf_draw_rect(curr_x + col * scale, y + row * scale, scale, scale, color);
        }
      }
    }
    curr_x += font->tile_size * scale;
  }
}
