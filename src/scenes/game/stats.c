#include "stats.h"

#include <engine/mathext.h>


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
    s.gunPowerRenderPos = 1.0f;

    s.score = 0;
    s.coins = 0;

    return s;
}


// Add coins (if room)
void stats_add_coins(Stats* s, int count) {

    s->coins = min_int32_2(99, s->coins + count);
}


// Modify gun power
void stats_change_gun_power(Stats* s, float delta) {

    s->gunPower += delta;
    s->gunPower = max_float_2(-1.0f, 
        min_float_2(1.0f, s->gunPower)
        );
}


// Add points
void stats_add_points(Stats* s, int points) {

    s->score += points;
}


// Update
void stats_update(Stats* s, float tm) {

    const float DELTA = 0.01f;

    // Update render positions of the different
    // bars
    if (s->gunPowerRenderPos < s->gunPower) {

        s->gunPowerRenderPos += DELTA * tm;
        if (s->gunPowerRenderPos > s->gunPower) {

            s->gunPowerRenderPos = s->gunPower;
        }
    }
    else if (s->gunPowerRenderPos > s->gunPower) {

        s->gunPowerRenderPos -= DELTA * tm;
        if (s->gunPowerRenderPos < s->gunPower) {

            s->gunPowerRenderPos = s->gunPower;
        }
    }
}