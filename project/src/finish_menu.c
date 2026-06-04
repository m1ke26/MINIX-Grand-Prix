#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "finish_menu.h"
#include "colors.h"
#include "video-card.h"
#include "kbc.h"





static void format_ticks(char *out, size_t out_size, unsigned ticks) {
    const unsigned TICKS_PER_SECOND = 60;
    unsigned total_seconds = ticks / TICKS_PER_SECOND;
    unsigned mins = total_seconds / 60;
    unsigned secs = total_seconds % 60;
    unsigned ms = (ticks % TICKS_PER_SECOND) * 1000 / TICKS_PER_SECOND;
    snprintf(out, out_size, "%02u:%02u.%03u", mins, secs, ms);
}

finish_menu_t *finish_menu_create(font_t *font, const race_t *race) {
    if (font == NULL || race == NULL) return NULL;

    finish_menu_t *menu = malloc(sizeof(finish_menu_t));
    if (menu == NULL) return NULL;

    menu->font = font;
    menu->final_ticks = race->ticks_elapsed;
    menu->lap_count = race->current_lap - 1;
    if (menu->lap_count > race->total_laps)
        menu->lap_count = race->total_laps;

    for (int i = 0; i < menu->lap_count; i++) {
        menu->lap_times[i] = race->lap_times[i];
    }

    return menu;
}

void finish_menu_destroy(finish_menu_t *menu) {
    if (menu == NULL) return;
    free(menu);
}

void finish_menu_draw(const finish_menu_t *menu) {
    if (menu == NULL || menu->font == NULL) return;

    vg_buf_draw_rect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BLACK);
    draw_centered_text(menu->font, "RACE FINISHED", 60, 4, COLOR_WHITE);

    char line[64];
    for (int i = 0; i < menu->lap_count; i++) {
        char lap_time_str[32];
        format_ticks(lap_time_str, sizeof(lap_time_str), menu->lap_times[i]);
        snprintf(line, sizeof(line), "Lap %d: %s", i + 1, lap_time_str);
        draw_string_scaled(menu->font, line, 220, 160 + i * 32, 2, COLOR_WHITE);
    }

    char total_time_str[32];
    format_ticks(total_time_str, sizeof(total_time_str), menu->final_ticks);
    snprintf(line, sizeof(line), "Total Time: %s", total_time_str);
    draw_string_scaled(menu->font, line, 220, 160 + menu->lap_count * 32 + 24, 2, COLOR_WHITE);
    draw_centered_text(menu->font, "PRESS ENTER TO CONTINUE", 520, 2, COLOR_WHITE);
}

bool finish_menu_handle_key(const finish_menu_t *menu, uint8_t scancode) {
    if (menu == NULL) return false;
    return scancode == ENTER_MAKE;
}
