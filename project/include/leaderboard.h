#ifndef _LEADERBOARD_H_
#define _LEADERBOARD_H_

#include <stdbool.h>
#include "rtc.h"

#define MAX_USERNAME_LEN 16

typedef struct {
    char username[MAX_USERNAME_LEN];
    unsigned time;              // Time in ticks (game ticks, e.g. 60 ticks = 1 second)
    int track_index;            // 0, 1, or 2
    rtc_date date;              // Date the record was set
} leaderboard_entry_t;

/**
 * @brief Saves a race result to leaderboard file.
 * @param username The player's username.
 * @param time The race time in ticks (use race->ticks_elapsed to preserve milliseconds).
 * @param track_index The track index (0-2).
 * @param date The current date from RTC.
 */
void leaderboard_save_time(const char *username, unsigned time, int track_index, rtc_date date);

/**
 * @brief Retrieves the best times for a given username across tracks.
 * @param username Player username to query.
 * @param out_times Pointer to an array of at least `num_tracks` unsigned elements to receive best times.
 * @param out_dates Pointer to an array of at least `num_tracks` rtc_date elements to receive dates. May be NULL.
 * @param num_tracks Number of tracks to fill (usually 3).
 * @return true if at least one record was found for the user, false otherwise.
 */
bool leaderboard_get_user_best(const char *username, unsigned out_times[], rtc_date out_dates[], int num_tracks);

#endif /* _LEADERBOARD_H_ */
