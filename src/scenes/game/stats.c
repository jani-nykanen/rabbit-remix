#include "stats.h"

// Create defaults stats
Stats create_default_stats() {

    const int MAX_LIVES = 5;
    const int DEF_LIVES = 3;

    Stats s;

    s.lives = DEF_LIVES;
    s.maxLives = MAX_LIVES;

    s.powerLevel = 0;
    s.powerMeter = 0.0f;
    s.gunPower = 1.0f;

    s.score = 0;

    return s;
}
