//
// Leaderboard
// (Offline version, sorry)
// (c) 2019 Jani Nykänen
//

#ifndef __LEADERBOARD__
#define __LEADERBOARD__

#define LB_NAME_LENGTH 16
#define LB_ENTRY_MAX 10

// Entry type
typedef struct {

    char name [LB_NAME_LENGTH];
    int score;

} Entry;

// Leaderboard type
typedef struct {

    // Entries
    Entry entries [LB_ENTRY_MAX];
    // Entry count (in the case this'll be dynamic)
    int count;

} Leaderboard;

// Create a leaderboard
Leaderboard create_leaderboard();

// Fetch scores
int lb_fetch_scores(Leaderboard* lb);

// Add a score
int lb_add_score(Leaderboard* lb, char* name, int count);

#endif // __LEADERBOARD__
