#include "matrix.h"

#include "math.h"


// Create an identity matrix
Mat4 mat4_identity() {

    Mat4 a;

    int x, y;
    for (y = 0; y < 4; ++ y) {

        for (x = 0; x < 4; ++ x) {

            a.m[x][y] = x == y ? 1 : 0;
        }
    }

    return a;
}


// Create a translation matrix
Mat4 mat4_translate(float x, float y, float z) {

    Mat4 a = mat4_identity();
    a.m[3][0] = x;
    a.m[3][1] = y;
    a.m[3][2] = z;
}


// Create a scaling matrix
Mat4 mat4_scale(float x, float y, float z) {

    Mat4 a = mat4_identity();
    a.m[0][0] = x;
    a.m[1][1] = y;
    a.m[2][2] = z;
}


// Create a rotation matrix
Mat4 mat4_rotate(float angle, float x, float y, float z) {

    float ca = cosf(angle*x);
    float sa = sinf(angle*x);

    float cb = cosf(angle*y);
    float sb = sinf(angle*y);

    float cc = cosf(angle*z);
    float sc = sinf(angle*z);

    Mat4 a = mat4_identity();

    a.m[0][0] = cb * cc; a.m[1][0] = -cb * sc; a.m[2][0] = sb;
    a.m[0][1] = sa * sb * cc + ca * sc; a.m[1][1] = -sa * sb * sc + ca * cc; a.m[2][1] = -sa * cb;
    a.m[0][2] = -ca * sb * cc; a.m[1][2] = ca * sb * sc + sa * cc; a.m[2][2] = ca * cb;

    return a;
}


// Multiply
Mat4 mat4_mul(Mat4 a, Mat4 b) {

    Mat4 out;
    int i, j;

    for (j = 0; j < 4; ++ j) {

        for (i = 0; i < 4; ++ i) {

            // ...
        }
    }

    return out;
}

