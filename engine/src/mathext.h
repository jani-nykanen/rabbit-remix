//
// Math extension
// (c) 2019 Jani Nykänen
//

#ifndef __MATHEXT__
#define __MATHEXT__

#include "types.h"

// Sort a point triplet by the second
// component, increasing
void sort_points_3(Point* points);

// Min & max
int min_int32_2(int a, int b);
int max_int32_2(int a, int b);
float min_float_2(float a, float b);
float max_float_2(float a, float b);
int min_int32_3(int a, int b, int c);
int max_int32_3(int a, int b, int c);

// Negative modulo
int neg_mod(int m, int n);

// Round fixed point number
int round_fixed(int n, int p);

#endif // __MATHEXT__
