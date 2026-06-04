#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "finish_menu.h"
#include "colors.h"
#include "video-card.h"
#include "kbc.h"


static void draw_centered_text(font_t *font, const char *text, int y, int scale, uint32_t color) {
    if (font == NULL || text == NULL) return;

    int width = (int) strlen(text) * 8 * scale;
    int x = (SCREEN_WIDTH - width) / 2;
    draw_string_scaled(font, text, x, y, scale, color);
}

finish_menu_t *finish_menu_create(font_t *font, const race_t *race) {
    if (font == NULL || race == NULL) return NULL;

    finish_menu_t *menu = malloc(sizeof(finish_menu_t));
    if (menu == NULL) return NULL;

    menu->font = font;
    menu->final_time = race->seconds_elapsed;
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
        int lap_secs = menu->lap_times[i] / 60;
        int mins = lap_secs / 60;
        int secs = lap_secs % 60;
        snprintf(line, sizeof(line), "Lap %d: %02d:%02d", i + 1, mins, secs);
        draw_string_scaled(menu->font, line, 220, 160 + i * 32, 2, COLOR_WHITE);
    }

    int total_mins = menu->final_time / 60;
    int total_secs = menu->final_time % 60;
    snprintf(line, sizeof(line), "Total Time: %02d:%02d", total_mins, total_secs);
    draw_string_scaled(menu->font, line, 220, 160 + menu->lap_count * 32 + 24, 2, COLOR_WHITE);
    draw_centered_text(menu->font, "PRESS ENTER TO CONTINUE", 520, 2, COLOR_WHITE);
}

bool finish_menu_handle_key(const finish_menu_t *menu, uint8_t scancode) {
    if (menu == NULL) return false;
    return scancode == ENTER_MAKE;
}
