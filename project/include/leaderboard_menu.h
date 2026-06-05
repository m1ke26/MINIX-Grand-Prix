#ifndef _LEADERBOARD_MENU_H_
#define _LEADERBOARD_MENU_H_

#include "font.h"
#include "rtc.h"

typedef struct {
    font_t *font;
    char username[16];
    unsigned times[3];
    rtc_date dates[3];
} leaderboard_menu_t;

/**
 * @brief Creates a leaderboard menu for the given username, loading their best times.
 * @param font The font to use for rendering the menu.
 * @param username The player's username to display and load times for.
 * @return Pointer to the created leaderboard menu, or NULL on failure.
 */
leaderboard_menu_t *leaderboard_menu_create(font_t *font, const char *username);

/**
 * @brief Frees resources associated with the leaderboard menu.
 * @param menu Pointer to the leaderboard menu to destroy.
 */
void leaderboard_menu_destroy(leaderboard_menu_t *menu);

/**
 * @brief Draws the leaderboard menu.
 * @param menu Pointer to the leaderboard menu to draw.
 */
void leaderboard_menu_draw(const leaderboard_menu_t *menu);

/**
 * @brief Handles a key press event for the leaderboard menu.
 * @param menu Pointer to the leaderboard menu.
 * @param scancode The scancode of the pressed key.
 * @return true if the key was handled, false otherwise.
 */
bool leaderboard_menu_handle_key(const leaderboard_menu_t *menu, uint8_t scancode);

#endif /* _LEADERBOARD_MENU_H_ */
