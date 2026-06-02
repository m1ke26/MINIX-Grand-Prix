#ifndef _INPUT_H_
#define _INPUT_H_

#include <stdbool.h>

typedef struct {
    bool accelerate;
    bool brake;
    bool turn_left;
    bool turn_right;
    bool handbrake;
    bool boost;
} game_input_t;

#endif /* _INPUT_H_ */
