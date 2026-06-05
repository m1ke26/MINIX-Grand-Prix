#include "hud.h"
#include "colors.h"
#include "video-card.h"
#include "font.h"
#include <stdio.h>
#include <string.h>

void hud_draw_boost_indicator(car_t *car) {
  if (car == NULL) return;

  int bar_width = 140;
  int bar_height = 14;
  int x = SCREEN_WIDTH  - bar_width - 20;
  int y = SCREEN_HEIGHT - bar_height - 20;
  double boost_ratio = car->boost_amount / CAR_BOOST_MAX;
  int fill_width = (int)(bar_width * boost_ratio);

  vg_buf_draw_rect(x - 2, y - 2, bar_width + 4, bar_height + 4, COLOR_HUD_BORDER);
  vg_buf_draw_rect(x, y, bar_width, bar_height, COLOR_HUD_BAR_BACKGROUND);
  vg_buf_draw_rect(x, y, fill_width, bar_height, COLOR_BOOST_FILL);
}

void hud_draw_race_start_prompt(font_t *font) {
  vg_buf_draw_rect(160, 196, 480, 112, COLOR_HUD_PANEL);
  draw_centered_text(font, "PRESS ENTER TO START", 216, 2, COLOR_WHITE);
  draw_centered_text(font, "GET READY", 280, 3, COLOR_COUNTDOWN_TEXT);
}

void hud_draw_race_countdown(font_t *font, int seconds_left) {
  char text[2];
  snprintf(text, sizeof(text), "%d", seconds_left);

  vg_buf_draw_rect(332, 190, 136, 104, COLOR_HUD_PANEL);
  draw_centered_text(font, text, 214, 8, COLOR_COUNTDOWN_TEXT);
}

void hud_draw_lap_counter(font_t *font, int current_lap, int total_laps) {
  char text[20];
  snprintf(text, sizeof(text), "Laps:%d/%d", current_lap, total_laps);
  draw_centered_text(font, text, 20, 2, COLOR_WHITE);
}

void hud_draw_timer(font_t *font, unsigned time_elapsed) {
  int mins = time_elapsed / 60;
  int secs = time_elapsed % 60;
  char text[12];
  snprintf(text, sizeof(text), "Time: %02d:%02d", mins, secs);
  draw_centered_text(font, text, 60, 2, COLOR_WHITE);
}

void hud_draw_controls_panel(font_t *font) {
  if (font == NULL) return;

  vg_buf_draw_rect(160, 320, 480, 260, COLOR_HUD_PANEL);
  draw_centered_text(font, "CONTROLS", 340, 3, COLOR_WHITE);
  draw_centered_text(font, "UP-W",     400, 2, COLOR_WHITE);
  draw_string_scaled(font, "LEFT-A",   290, 430, 2, COLOR_WHITE);
  draw_string_scaled(font, "RIGHT-D",  420, 430, 2, COLOR_WHITE);
  draw_centered_text(font, "DOWN-S",   460, 2, COLOR_WHITE);
  draw_string_scaled(font, "NITRO-SHIFT", 170, 520, 2, COLOR_WHITE);
  draw_string_scaled(font, "DRIFT-SPACE", 450, 520, 2, COLOR_WHITE);
}
