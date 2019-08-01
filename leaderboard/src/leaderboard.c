#include "leaderboard.h"

#include <stdio.h>
#include <stdlib.h>


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

    // Return something, for testing purposes
    int i;
    for (i = 0; i < LB_ENTRY_MAX; ++ i) {

        snprintf(lb->entries[i].name, LB_NAME_LENGTH, "DEFAULT");
        lb->entries[i].score = 10000 - i*1000;
    }

    return 0;
}
