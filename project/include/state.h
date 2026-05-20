#ifndef _STATE_H_
#define _STATE_H_

#include <stdbool.h>
#include "car.h"

typedef enum {
    STATE_START,
    STATE_IN_GAME,
    STATE_GAME_OVER
} state_tag_t;

typedef struct {
    state_tag_t tag;
    union {
        struct {
            int cursor_x;
            int cursor_y;
        } start;

        struct {
            car_t car;
            unsigned laps;
            unsigned seconds_elapsed;
        } in_game;

        struct {
            unsigned final_time;
            unsigned laps_done;
        } game_over;
    } data;
} state_t;

#endif /* _STATE_H_ */
