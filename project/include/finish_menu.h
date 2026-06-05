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

finish_menu_t *finish_menu_create(font_t *font, const race_t *race);
void finish_menu_destroy(finish_menu_t *menu);
void finish_menu_draw(const finish_menu_t *menu);
bool finish_menu_handle_key(const finish_menu_t *menu, uint8_t scancode);

#endif /* _FINISH_MENU_H_ */
