#ifndef _RACE_H_
#define _RACE_H_

#include <stdbool.h>

typedef struct {
    bool countdown_started;
    int countdown_ticks;
    int current_lap; // For future use, not currently implemented´
    int total_laps; // For future use, not currently implemented
    int seconds_elapsed; // For future use, not currently implemented
} race_t;

/**
 * @brief Initializes the race structure with default values.
 */
void race_init(race_t *race);

/**
 * @brief Starts the race countdown if it hasn't already started and if the countdown is not already in progress
 */

void race_start_countdown(race_t *race);

/**
 * @brief Updates the race countdown timer, should be called every game tick.
 */
void race_update(race_t *race);

/**
 * @brief Checks if the race has officially started (countdown finished).
 * @returns true if the race has started, false otherwise.
 */
bool race_has_started(const race_t *race);

/**
 * @brief Checks if the race countdown has started (but may not have finished).
 * @returns true if the countdown has started, false otherwise.
 */
bool race_countdown_started(const race_t *race);

/**
 * @brief Gets the number of seconds left in the race countdown.
 * @returns The number of seconds left in the countdown.
 */
int race_countdown_seconds_left(const race_t *race);

#endif /* _RACE_H_ */
