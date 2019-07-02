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
