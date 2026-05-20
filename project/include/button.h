#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "sprite.h"
#include "font.h"
#include <stdbool.h>

#define BTN_W 200
#define BTN_H  60
#define BORDER  3

typedef enum {
  PURE_WHITE = 0xffffff,
  MILD_GREEN = 0x00008800,
  DARK_GRAY  = 0x555555,
  CLASSIC_SILVER = 0xCCCCCC
} button_color_t;

typedef struct {
  sprite_t *sp;
  sprite_t *hover_sp;
  int x, y;
  char text[100];
  font_t *font;
  button_color_t back_color;
  button_color_t hover_frame_color;
} button_t;

/**
 * @brief Creates a button.
 */
button_t* button_create(font_t *font, const char *text, int x, int y, xpm_map_t normal_xpm, xpm_map_t hover_xpm);

/**
 * @brief Destroys a button.
 */
void button_destroy(button_t *b);

/**
 * @brief Draws the button.
 * @param b Pointer to the button.
 * @param hover Whether the button is being hovered.
 */
void button_draw(button_t *b, bool hover);

/**
 * @brief Checks if a position is inside the button.
 */
bool button_is_hovered(button_t *b, int x, int y);

#endif /* _BUTTON_H_ */
