#ifndef _PAUSE_MENU_H_
#define _PAUSE_MENU_H_

#include "button.h"
#include "font.h"
#include "video-card.h"

typedef struct {
  font_t *font;
  button_t *resume_btn;
  button_t *exit_btn;
} pause_menu_t;

/**
 * @brief Initializes the pause menu and its buttons.
 */
pause_menu_t* pause_menu_create(font_t *font);

/**
 * @brief Frees pause menu resources.
 */
void pause_menu_destroy(pause_menu_t *pm);

/**
 * @brief Draws all pause menu elements.
 * @param pm Pointer to the pause menu.
 * @param cursor_x Current mouse x position.
 * @param cursor_y Current mouse y position.
 */
void pause_menu_draw(pause_menu_t *pm, int cursor_x, int cursor_y);

#endif /* _PAUSE_MENU_H_ */
