#include "stats.h"

#include <engine/mathext.h>

#include <stdio.h>
#include <math.h>

// Constants
static const int DIGIT_COUNT = 6;


// Update score string
static void update_score_string(Stats* s) {

    int zeroCount = 0;
    int i;
    char zeroes [SCORE_STR_MAX_LEN];

    for (i = 1 ; i < DIGIT_COUNT; ++ i) {

        if (s->score < (int)powf(10, i) ) {

            zeroes[zeroCount ++] = '0';
        }
    }
    zeroes[zeroCount] = '\0';
    
    snprintf(s->scoreStr, 
        SCORE_STR_MAX_LEN, "%s%d", zeroes, s->score);
}


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
    s.oldScore = s.score;
    s.coins = 0;

    // Set score string
    update_score_string(&s);

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

    // Check if score has changed
    if (s->score != s->oldScore) {

        update_score_string(s);
    }
    s->oldScore = s->score;
}
