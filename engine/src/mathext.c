#include "mathext.h"

#include <stdbool.h>


// Sort a point triplet by the second
// component, increasing
void sort_points_3(Point* points) {

    int i;

    Point buf;
    bool changed = false;
    do {

        changed = false;
        for(i=1; i < 3; ++ i) {

            if (points[i].y < points[i-1].y) {

                buf = points[i];
                points[i] = points[i-1];
                points[i-1] = buf;
                changed = true;
                break;
            }
            
        }

    }
    while(changed);

}


// Minimum (out of 2)
int min_int32_2(int a, int b) {

    return a <= b ? a : b;
}


// Maximum (out of 2)
int max_int32_2(int a, int b) {

    return a >= b ? a : b;
}


// And the same for float
// TODO: Add a macro to get rid of repeating
// code
float min_float_2(float a, float b) {

    return a <= b ? a : b;
}
float max_float_2(float a, float b) {

    return a >= b ? a : b;
}


// Minimum (out of 3)
int min_int32_3(int a, int b, int c) {

    return (a <= b && a <= c) ? a 
        : ( (b <= a && b <=c) ? b : c);
}


// Maximum (out of 3)
int max_int32_3(int a, int b, int c) {

    return (a >= b && a >= c) ? a 
        : ( (b >= a && b >=c) ? b : c);
}


// Negative modulo
int neg_mod(int m, int n) {

    // if (n == 0) return 0;

    if(m < 0) {

        return n - (-m % n);
    }
    return m % n;
}


// Round fixed point number
int round_fixed(int n, int p) {

    int m = neg_mod(n, p);
    if (m < p/2) {

        return n / p;
    }
    else {

        return n / p +1;
    }
}
