//
// Stats
// (c) 2019 Jani Nykänen
//


#ifndef __STATS__
#define __STATS__

#include <engine/types.h>

#define SCORE_STR_MAX_LEN 8

// Stats type
typedef struct {

    int lives;
    int maxLives;
    float powerMeter;
    float powerMeterRenderPos;
    int powerLevel;
    int score;
    int oldScore;
    int coins;
    float bonus;
    float gunPower;
    float gunPowerRenderPos;

    // Score string (let's put it here so
    // we do not have to regenerate it every
    // frame)
    char scoreStr [SCORE_STR_MAX_LEN];

} Stats;

// Create defaults stats
Stats create_default_stats();

// Add coins (if room)
void stats_add_coins(Stats* s, int count);

// Modify gun power
void stats_modify_gun_power(Stats* s, float delta);

// Modify power meter
void stats_modify_power(Stats* s, float delta);

// Reset power 
void stats_reset_power(Stats* s);

// Add points
void stats_add_points(Stats* s, int points);

// Add a life
void stats_add_life(Stats* s);

// Update
void stats_update(Stats* s, float tm);

#endif // __STATS__
