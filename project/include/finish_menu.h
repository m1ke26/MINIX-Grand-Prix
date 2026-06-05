#ifndef _FINISH_MENU_H_
#define _FINISH_MENU_H_

#include <stdbool.h>
#include <stdint.h>
#include "font.h"
#include "race.h"
#include "video-card.h"

typedef struct finish_menu {
    font_t *font;
    unsigned final_ticks;
    int lap_times[16];
    int lap_count;
} finish_menu_t;

/**
 * @brief Creates a finish menu with the given font and race data.
 * @param font Pointer to the font to use for rendering.
 * @param race Pointer to the race data.
 * @return Pointer to the created finish menu.
 */
finish_menu_t *finish_menu_create(font_t *font, const race_t *race);

/**
 * @brief Destroys the finish menu and frees any allocated resources.
 * @param menu Pointer to the finish menu to destroy.
 */
void finish_menu_destroy(finish_menu_t *menu);

/**
 * @brief Draws the finish menu on the screen.
 * @param menu Pointer to the finish menu to draw.
 */
void finish_menu_draw(const finish_menu_t *menu);

/**
 * @brief Handles a key press event for the finish menu.
 * @param menu Pointer to the finish menu.
 * @param scancode The scancode of the pressed key.
 * @return True if the key was handled, false otherwise.
 */
bool finish_menu_handle_key(const finish_menu_t *menu, uint8_t scancode);

#endif /* _FINISH_MENU_H_ */
