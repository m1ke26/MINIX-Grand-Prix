#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "colors.h"
#include "font.h"
#include "video-card.h"
#include <stdbool.h>
#include <stdint.h>

#define BTN_W   200
#define BTN_H    60
#define BORDER    3

typedef struct {
  int x, y;
  char text[100];
  font_t *font;
  uint32_t color;        /**< Normal background color  */
  uint32_t hover_color;  /**< Hovered background color */
  uint32_t border_color; /**< Border color             */
} button_t;

/**
 * @brief Creates a button drawn as a plain pixel rectangle.
 * @param font         Font used for the button label.
 * @param text         Label text.
 * @param x            Top-left X position.
 * @param y            Top-left Y position.
 * @param color        Normal background color.
 * @param hover_color  Hovered background color.
 * @param border_color Border color.
 */
button_t* button_create(font_t *font, const char *text, int x, int y,
                        uint32_t color, uint32_t hover_color,
                        uint32_t border_color);

/**
 * @brief Destroys a button.
 */
void button_destroy(button_t *b);

/**
 * @brief Draws the button as a filled rectangle with a border.
 * @param b     Pointer to the button.
 * @param hover Whether the button is being hovered.
 */
void button_draw(button_t *b, bool hover);

/**
 * @brief Checks if a position is inside the button.
 */
bool button_is_hovered(button_t *b, int x, int y);

#endif /* _BUTTON_H_ */
