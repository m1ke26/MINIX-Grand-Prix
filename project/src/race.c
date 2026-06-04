#include "race.h"
#include <stdio.h>

#define RACE_COUNTDOWN_SECONDS 3
#define TIMER_TICKS_PER_SECOND 60
#define RACE_COUNTDOWN_TICKS (RACE_COUNTDOWN_SECONDS * TIMER_TICKS_PER_SECOND)

void race_init(race_t *race) {
    if (race == NULL) return;

    race->countdown_started = false;
    race->countdown_ticks = 0;
}

void race_start_countdown(race_t *race) {
    if (race == NULL || race_has_started(race) || race->countdown_ticks > 0) return;

    race->countdown_started = true;
    race->countdown_ticks = RACE_COUNTDOWN_TICKS;
}

void race_update(race_t *race) {
    if (race == NULL || race_has_started(race) || race->countdown_ticks <= 0) return;

    race->countdown_ticks--;
}

bool race_has_started(const race_t *race) {
    return race != NULL && race->countdown_started && race->countdown_ticks <= 0;
}

bool race_countdown_started(const race_t *race) {
    return race != NULL && race->countdown_started;
}

int race_countdown_seconds_left(const race_t *race) {
    if (race == NULL) return 0;
    return (race->countdown_ticks + TIMER_TICKS_PER_SECOND - 1) / TIMER_TICKS_PER_SECOND;
}
