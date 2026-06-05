#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "leaderboard_menu.h"
#include "colors.h"
#include "video-card.h"
#include "leaderboard.h"
#include "kbc.h"
#include "race.h"

leaderboard_menu_t *leaderboard_menu_create(font_t *font, const char *username) {
    leaderboard_menu_t *menu = malloc(sizeof(leaderboard_menu_t));
    if (!menu) return NULL;
    menu->font = font;
    menu->username[0] = '\0';
    if (username != NULL) {
        strncpy(menu->username, username, sizeof(menu->username)-1);
        menu->username[sizeof(menu->username)-1] = '\0';
    }
    // initialize times and dates
    for (int i = 0; i < 3; ++i) {
        menu->times[i] = 0;
        menu->dates[i].day = menu->dates[i].month = menu->dates[i].year = 0;
    }

    // Fill times and dates from leaderboard
    leaderboard_get_user_best(menu->username, menu->times, menu->dates, 3);

    return menu;
}

void leaderboard_menu_destroy(leaderboard_menu_t *menu) {
    if (menu == NULL) return;
    free(menu);
}

void leaderboard_menu_draw(const leaderboard_menu_t *menu) {
    if (menu == NULL || menu->font == NULL) return;

    vg_buf_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_MENU_BACKGROUND);
    char title[64];
    snprintf(title, sizeof(title), "%s's Best Times", menu->username[0] ? menu->username : "Player");
    draw_centered_text(menu->font, title, 80, 3, COLOR_MENU_TITLE);

    char line[64];
    for (int i = 0; i < 3; ++i) {
        if (menu->times[i] == 0) {
            snprintf(line, sizeof(line), "Track %d: --:--.---", i+1);
            draw_centered_text(menu->font, line, 160 + i * 60, 2, COLOR_MENU_TEXT);
        } else {
            char time_str[32];
            race_format_time(time_str, sizeof(time_str), menu->times[i]);
            snprintf(line, sizeof(line), "Track %d: %s", i+1, time_str);
            draw_centered_text(menu->font, line, 160 + i * 60, 2, COLOR_MENU_TEXT);

            char date_str[32];
            snprintf(date_str, sizeof(date_str), "%02d/%02d/%02d",
                     menu->dates[i].day, menu->dates[i].month, menu->dates[i].year);
            draw_centered_text(menu->font, date_str, 180 + i * 60, 2, COLOR_MENU_TEXT);
        }
    }

    draw_centered_text(menu->font, "PRESS ENTER TO RETURN", 540, 2, COLOR_WHITE);
}

bool leaderboard_menu_handle_key(const leaderboard_menu_t *menu, uint8_t scancode) {
    if (menu == NULL) return false;
    return scancode == ENTER_MAKE || scancode == ESC_MAKE;
}
