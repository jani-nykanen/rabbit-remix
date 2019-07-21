//
// Stats
// (c) 2019 Jani Nykänen
//


#ifndef __STATS__
#define __STATS__

#include <engine/types.h>

// Stats type
typedef struct {

    int lives;
    int maxLives;
    float powerMeter;
    int powerLevel;
    int score;
    int coins;
    float bonus;
    float gunPower;
    float gunPowerRenderPos;

} Stats;

// Create defaults stats
Stats create_default_stats();

// Add coins (if room)
void stats_add_coins(Stats* s, int count);

// Modify gun power
void stats_change_gun_power(Stats* s, float delta);

// Add points
void stats_add_points(Stats* s, int points);

// Update
void stats_update(Stats* s, float tm);

#endif // __STATS__
