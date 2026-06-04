#ifndef _LEADERBOARD_H_
#define _LEADERBOARD_H_

#include "rtc.h"

#define MAX_USERNAME_LEN 16

typedef struct {
    char username[MAX_USERNAME_LEN];
    unsigned time;              // Time in seconds
    int track_index;            // 0, 1, or 2
    rtc_date date;              // Date the record was set
} leaderboard_entry_t;

/**
 * @brief Saves a race result to leaderboard file.
 * @param username The player's username.
 * @param time The race time in seconds.
 * @param track_index The track index (0-2).
 * @param date The current date from RTC.
 */
void leaderboard_save_time(const char *username, unsigned time, int track_index, rtc_date date);

#endif /* _LEADERBOARD_H_ */
