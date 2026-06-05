#ifndef _HUD_H_
#define _HUD_H_

#include <lcom/lcf.h>
#include "car.h"
#include "font.h"
#include "video-card.h"

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

/**
 * @brief Draws the lap counter on the screen.
 * @param font Pointer to the font to use for drawing the text.
 * @param current_lap The current lap.
 * @param total_laps The total number of laps.
**/
void hud_draw_lap_counter(font_t *font, int current_lap, int total_laps);

/**
 * @brief Draws the timer on the screen.
 * @param font Pointer to the font to use for drawing the text.
 * @param time_elapsed The time elapsed since the start of the race.
**/
void hud_draw_timer(font_t *font, unsigned time_elapsed);

/**
 * @brief Draws the controls reference panel shown before the race starts.
 * @param font Pointer to the font to use for drawing the text.
**/
void hud_draw_controls_panel(font_t *font);

#endif /* _HUD_H_ */
