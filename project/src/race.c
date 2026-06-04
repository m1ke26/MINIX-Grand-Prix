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
    race->next_checkpoint = CHECKPOINT_1;
    race->checkpoint_armed = false;
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

static void race_advance_checkpoint(race_t *race, terrain_type_t hit) {
    if (hit == START) {
        race->current_lap++;
        race->next_checkpoint = CHECKPOINT_1;
    } else if (hit == CHECKPOINT_1) {
        race->next_checkpoint = CHECKPOINT_2;
    } else if (hit == CHECKPOINT_2) {
        race->next_checkpoint = CHECKPOINT_3;
    } else if (hit == CHECKPOINT_3) {
        race->next_checkpoint = START;
    }
}

void race_check_checkpoints(race_t *race, track_t *track, double car_x, double car_y, int car_w, int car_h) {
    if (race == NULL || track == NULL || race_is_finished(race))
        return;

    terrain_type_t hit = track_car_checkpoint(track, (int)car_x, (int)car_y, car_w, car_h);

    if (hit == TERRAIN_ROAD) {
        race->checkpoint_armed = true;
        return;
    }

    if (!race->checkpoint_armed || hit != race->next_checkpoint)
        return;

    race->checkpoint_armed = false;
    race_advance_checkpoint(race, hit);
}
