#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "leaderboard.h"

#define LEADERBOARD_FILE "leaderboard.dat"
#define MAX_ENTRIES 100

void leaderboard_save_time(const char *username, unsigned time, int track_index, rtc_date date) {
    if (username == NULL) return;
    
    leaderboard_entry_t entries[MAX_ENTRIES];
    int count = 0;
    
    // Load existing entries
    FILE *file = fopen(LEADERBOARD_FILE, "rb");
    if (file != NULL) {
        count = fread(entries, sizeof(leaderboard_entry_t), MAX_ENTRIES, file);
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
    file = fopen(LEADERBOARD_FILE, "wb");
    if (file != NULL) {
        fwrite(entries, sizeof(leaderboard_entry_t), count, file);
        fclose(file);
    }
}
