#include "hud.h"
#include "colors.h"
#include "video-card.h"
#include <stdio.h>
#include <string.h>

static void hud_draw_centered_text(font_t *font, const char *text, int y, int scale, uint32_t color) {
  if (font == NULL || text == NULL) return;

  int width = (int) strlen(text) * 8 * scale;
  int x = (SCREEN_WIDTH - width) / 2;
  draw_string_scaled(font, text, x, y, scale, color);
}

void hud_draw_boost_indicator(car_t *car) {
  if (car == NULL) return;

  int bar_width = 140;
  int bar_height = 14;
  int x = 800 - bar_width - 20;
  int y = 600 - bar_height - 20;
  double boost_ratio = car->boost_amount / CAR_BOOST_MAX;
  int fill_width = (int)(bar_width * boost_ratio);

  vg_buf_draw_rect(x - 2, y - 2, bar_width + 4, bar_height + 4, COLOR_HUD_BORDER);
  vg_buf_draw_rect(x, y, bar_width, bar_height, COLOR_HUD_BAR_BACKGROUND);
  vg_buf_draw_rect(x, y, fill_width, bar_height, COLOR_BOOST_FILL);
}

void hud_draw_race_start_prompt(font_t *font) {
  vg_buf_draw_rect(160, 196, 480, 112, COLOR_HUD_PANEL);
  hud_draw_centered_text(font, "PRESS ENTER TO START", 216, 2, COLOR_WHITE);
  hud_draw_centered_text(font, "GET READY", 280, 3, COLOR_COUNTDOWN_TEXT);
}

void hud_draw_race_countdown(font_t *font, int seconds_left) {
  char text[2];
  snprintf(text, sizeof(text), "%d", seconds_left);

  vg_buf_draw_rect(332, 190, 136, 104, COLOR_HUD_PANEL);
  hud_draw_centered_text(font, text, 214, 8, COLOR_COUNTDOWN_TEXT);
}
