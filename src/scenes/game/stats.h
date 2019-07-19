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

} Stats;

// Create defaults stats
Stats create_default_stats();

#endif // __STATS__
