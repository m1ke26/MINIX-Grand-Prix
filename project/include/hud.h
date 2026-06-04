#ifndef _HUD_H_
#define _HUD_H_

#include <lcom/lcf.h>
#include "car.h"
#include "font.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

/** 
 * @brief Draws the boost indicator for the car.
 * @param car Pointer to the car for which to draw the indicator.
**/
void hud_draw_boost_indicator(car_t *car);

/**
 * @brief Draws the race start prompt on the screen.
 * @param font Pointer to the font to use for drawing the text.
**/
void hud_draw_race_start_prompt(font_t *font);

/**
 * @brief Draws the race countdown number on the screen.
 * @param font Pointer to the font to use for drawing the text.
 * @param seconds_left The number of seconds left in the countdown.
**/
void hud_draw_race_countdown(font_t *font, int seconds_left);

#endif /* _HUD_H_ */
