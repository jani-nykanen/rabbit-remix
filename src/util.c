#include "util.h"

#include <stdio.h>
#include <math.h>


// Get score string
void get_score_string(char* buf, int s, int digits) {

    const int MAX_DIGITS = 32;

    if (digits > MAX_DIGITS)
        return;

    int zeroCount = 0;
    int i;
    char zeroes [32 +1];

    for (i = 1 ; i < digits; ++ i) {

        if (s < (int)powf(10, i) ) {

            zeroes[zeroCount ++] = '0';
        }
    }
    zeroes[zeroCount] = '\0';
    
    snprintf(buf, digits+1, "%s%d", zeroes, s);
}
