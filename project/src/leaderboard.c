#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include "leaderboard.h"

// 1. Changed extension to .txt
#define LEADERBOARD_FILE "/home/lcom/labs/grupo_2leic01_2/project/leaderboard.txt"
#define MAX_ENTRIES 100

bool leaderboard_get_user_best(const char *username, unsigned out_times[], int num_tracks) {
    if (username == NULL || out_times == NULL || num_tracks <= 0) return false;

    // Initialize outputs to 0 (meaning no record)
    for (int i = 0; i < num_tracks; ++i)
        out_times[i] = 0;

    FILE *file = fopen(LEADERBOARD_FILE, "r");
    if (file == NULL) return false;

    char line[256];
    bool found_any = false;
    while (fgets(line, sizeof(line), file) != NULL) {
        int day=0, month=0, year=0;
        unsigned temp_time = 0;
        int temp_track = 0;
        char temp_name[MAX_USERNAME_LEN] = {0};

        if (sscanf(line, "\"%15[^\"]\" %u %d %d %d %d", temp_name, &temp_time, &temp_track,
                   &day, &month, &year) == 6 ||
            sscanf(line, "%15s %u %d %d %d %d", temp_name, &temp_time, &temp_track,
                   &day, &month, &year) == 6) {
            if (strcmp(temp_name, username) == 0 && temp_track >= 0 && temp_track < num_tracks) {
                if (out_times[temp_track] == 0 || temp_time < out_times[temp_track]) {
                    out_times[temp_track] = temp_time;
                }
                found_any = true;
            }
        }
    }

    fclose(file);
    return found_any;
}

void leaderboard_save_time(const char *username, unsigned time, int track_index, rtc_date date) {
    if (username == NULL) return;
    if (username[0] == '\0') return; // don't save empty names
    
    leaderboard_entry_t entries[MAX_ENTRIES];
    int count = 0;
    
    // Load existing entries
    FILE *file = fopen(LEADERBOARD_FILE, "r");
    if (file != NULL) {
        char line[256];
        while (count < MAX_ENTRIES && fgets(line, sizeof(line), file) != NULL) {
            int temp_day = 0, temp_month = 0, temp_year = 0;
            unsigned temp_time = 0;
            int temp_track = 0;
            char temp_name[MAX_USERNAME_LEN] = {0};

            // Try quoted username first, then fallback to unquoted single-token names.
            if (sscanf(line, "\"%15[^\"]\" %u %d %d %d %d", temp_name, &temp_time, &temp_track,
                       &temp_day, &temp_month, &temp_year) == 6 ||
                sscanf(line, "%15s %u %d %d %d %d", temp_name, &temp_time, &temp_track,
                       &temp_day, &temp_month, &temp_year) == 6) {
                strncpy(entries[count].username, temp_name, MAX_USERNAME_LEN - 1);
                entries[count].username[MAX_USERNAME_LEN - 1] = '\0';
                entries[count].time = temp_time;
                entries[count].track_index = temp_track;
                entries[count].date.day = (uint8_t)temp_day;
                entries[count].date.month = (uint8_t)temp_month;
                entries[count].date.year = (uint8_t)temp_year;
                count++;
            }
        }
        fclose(file);
    }
    
    // Check if entry exists and update it, or add new one
    bool found = false;
    for (int i = 0; i < count; i++) {
        if (strcmp(entries[i].username, username) == 0 && entries[i].track_index == track_index) {
            if (time < entries[i].time) {
                entries[i].time = time;
                entries[i].date = date;
            }
            found = true;
            break;
        }
    }
    
    // Add new entry if not found and there's space
    if (!found && count < MAX_ENTRIES) {
        strncpy(entries[count].username, username, MAX_USERNAME_LEN - 1);
        entries[count].username[MAX_USERNAME_LEN - 1] = '\0';
        entries[count].time = time;
        entries[count].track_index = track_index;
        entries[count].date = date;
        count++;
    }
    
    // Save back to file
    file = fopen(LEADERBOARD_FILE, "w");
    if (file != NULL) {
        for (int i = 0; i < count; i++) {
            fprintf(file, "\"%s\" %u %d %d %d %d\n",
                    entries[i].username,
                    entries[i].time,
                    entries[i].track_index,
                    entries[i].date.day,
                    entries[i].date.month,
                    entries[i].date.year);
        }
        fclose(file);
    } else {
        int err = errno;
        fprintf(stderr, "leaderboard: failed to open '%s' for writing: %s\n", LEADERBOARD_FILE, strerror(err));
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            fprintf(stderr, "leaderboard: current working directory is '%s'\n", cwd);
        }
    }
}
