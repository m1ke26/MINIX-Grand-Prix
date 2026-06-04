#include "race.h"
#include "track.h"
#include <stdio.h>

#define RACE_COUNTDOWN_SECONDS 3
#define TIMER_TICKS_PER_SECOND 60
#define RACE_COUNTDOWN_TICKS (RACE_COUNTDOWN_SECONDS * TIMER_TICKS_PER_SECOND)

void race_init(race_t *race) {
    if (race == NULL) return;

    race->countdown_started = false;
    race->countdown_ticks = 0;
    race->current_lap = 1;
    race->total_laps = 3;
    race->seconds_elapsed = 0;
    race->ticks_elapsed = 0;
    race->next_checkpoint = 1; // Hitting checkpoint 0 completes a lap, so next to hit is 1
}

void race_start_countdown(race_t *race) {
    if (race == NULL || race_has_started(race) || race->countdown_ticks > 0) return;

    race->countdown_started = true;
    race->countdown_ticks = RACE_COUNTDOWN_TICKS;
}

void race_update(race_t *race) {
    if (race == NULL) return;

    if (race->countdown_started && race->countdown_ticks > 0) {
        race->countdown_ticks--;
    }
    else if (race_has_started(race)) {
        race->ticks_elapsed++;
        race->seconds_elapsed = race->ticks_elapsed / TIMER_TICKS_PER_SECOND;
    }
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


bool race_is_finished(const race_t *race) {
    return race != NULL && race->current_lap > race->total_laps;
}
