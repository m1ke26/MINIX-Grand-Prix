#include "pause_menu.h"
#include "colors.h"

pause_menu_t* pause_menu_create(font_t *font) {
    pause_menu_t *pm = malloc(sizeof(pause_menu_t));
    if (pm == NULL) return NULL;

    pm->font = font;
    pm->resume_btn = button_create(font, "RESUME", 300, 250, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);
    pm->exit_btn   = button_create(font, "EXIT",   300, 350, BTN_DARK_NAVY, BTN_LIGHT_NAVY, BTN_WHITE, BTN_SHAPE_RECT);

    if (pm->resume_btn == NULL || pm->exit_btn == NULL) {
        pause_menu_destroy(pm);
        return NULL;
    }

    return pm;
}


void pause_menu_destroy(pause_menu_t *pm) {
    if (pm == NULL) return; 
    button_destroy(pm->resume_btn);
    button_destroy(pm->exit_btn); 
    free(pm);
}

void pause_menu_draw(pause_menu_t *pm, int cursor_x, int cursor_y) {
    if (pm == NULL) return;

    // Draw "Paused" title
    if (pm->font != NULL)
        draw_string_scaled(pm->font, "PAUSED", 328, 130, 3, COLOR_MENU_TITLE);

    // Draw buttons
    if (pm->resume_btn != NULL)
        button_draw(pm->resume_btn, button_is_hovered(pm->resume_btn, cursor_x, cursor_y));
    if (pm->exit_btn != NULL)
        button_draw(pm->exit_btn, button_is_hovered(pm->exit_btn, cursor_x, cursor_y));
}

pause_menu_action_t pause_menu_handle_click(pause_menu_t *pm, int x, int y) {
    if (pm == NULL) return PAUSE_MENU_ACTION_NONE;

    if (button_is_hovered(pm->resume_btn, x, y))
        return PAUSE_MENU_ACTION_RESUME;
    if (button_is_hovered(pm->exit_btn, x, y))
        return PAUSE_MENU_ACTION_EXIT;

    return PAUSE_MENU_ACTION_NONE;
}
