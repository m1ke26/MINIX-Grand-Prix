#include "hud.h"
#include "video-card.h"

void hud_draw_boost_indicator(car_t *car) {
  if (car == NULL) return;

  int bar_width = 140;
  int bar_height = 14;
  int x = 800 - bar_width - 20;
  int y = 600 - bar_height - 20;
  double boost_ratio = car->boost_amount / CAR_BOOST_MAX;
  int fill_width = (int)(bar_width * boost_ratio);

  vg_buf_draw_rect(x - 2, y - 2, bar_width + 4, bar_height + 4, 0x000000);
  vg_buf_draw_rect(x, y, bar_width, bar_height, 0x1A1A1A);
  vg_buf_draw_rect(x, y, fill_width, bar_height, 0x007BFF);
}
