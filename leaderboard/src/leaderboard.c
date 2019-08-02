#include "leaderboard.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>


// Create a leaderboard
Leaderboard create_leaderboard() {

    Leaderboard lb;
    
    // Set, for testing purposes
    int i;
    for (i = 0; i < LB_ENTRY_MAX; ++ i) {

        snprintf(lb.entries[i].name, LB_NAME_LENGTH, "DEFAULT");
        lb.entries[i].score = 10000 - i*1000;
    }

    return lb;
}


// Fetch scores
int lb_fetch_scores(Leaderboard* lb) {

    SDL_Delay(2000);

    // Return something, for testing purposes
    int i;
    for (i = 0; i < LB_ENTRY_MAX; ++ i) {

        snprintf(lb->entries[i].name, LB_NAME_LENGTH, "DEFAULT");
        lb->entries[i].score = 10000 - i*1000;
    }

    return 0;
}


// Add a score
int lb_add_score(Leaderboard* lb, char* name, int count) {

    return 0;
}
