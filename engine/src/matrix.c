#include "matrix.h"

#include <math.h>



// Create a matrix full of zeroes
Matrix4 mat4_zeroes() {

    Matrix4 a;

    int x, y;
    for (y = 0; y < 4; ++ y) {

        for (x = 0; x < 4; ++ x) {

            a.m[x][y] = 0;
        }
    }

    return a;
}


// Create an identity matrix
Matrix4 mat4_identity() {

    Matrix4 a = mat4_zeroes();
    int i = 0;
    for(; i < 4; ++ i)
        a.m[i][i] = 1;

    return a;
}


// Create a translation matrix
Matrix4 mat4_translate(float x, float y, float z) {

    Matrix4 a = mat4_identity();
    a.m[3][0] = x;
    a.m[3][1] = y;
    a.m[3][2] = z;

    return a;
}


// Create a scaling matrix
Matrix4 mat4_scale(float x, float y, float z) {

    Matrix4 a = mat4_identity();
    a.m[0][0] = x;
    a.m[1][1] = y;
    a.m[2][2] = z;

    return a;
}


// Create a rotation matrix
Matrix4 mat4_rotate(float angle, float x, float y, float z) {

    float ca = cosf(angle*x);
    float sa = sinf(angle*x);

    float cb = cosf(angle*y);
    float sb = sinf(angle*y);

    float cc = cosf(angle*z);
    float sc = sinf(angle*z);

    Matrix4 a = mat4_identity();

    a.m[0][0] = cb * cc; a.m[1][0] = -cb * sc; a.m[2][0] = sb;
    a.m[0][1] = sa * sb * cc + ca * sc; a.m[1][1] = -sa * sb * sc + ca * cc; a.m[2][1] = -sa * cb;
    a.m[0][2] = -ca * sb * cc; a.m[1][2] = ca * sb * sc + sa * cc; a.m[2][2] = ca * cb;

    return a;
}


// Create a perspective matrix
Matrix4 mat4_perspective(float fovY, float aspect, float near, float far) {

    Matrix4 a = mat4_identity();

    float f = 1.0f / tanf(fovY / 2.0f);
    float nf = 1.0f / (near - far);
    
    a.m[0][0] = f / aspect;
    a.m[1][1] = f;
    a.m[2][2] = (far + near) * nf;
    a.m[3][2] = 2 * near * far * nf;

    return a;
}


// Multiply
Matrix4 mat4_mul(Matrix4 a, Matrix4 b) {

    Matrix4 out = mat4_zeroes();
    int i, j, k;

    // Row
    for (i = 0; i < 4; ++ i) {

        // Column
        for (j = 0; j < 4; ++ j) {

            for(k = 0; k < 4; ++ k) {

                out.m[j][i] += a.m[k][i] * b.m[j][k];
            }
        }
    }

    return out;
}


// Multiply with a vector
Vector3 mat4_mul_vec3(Matrix4 a, Vector3 b) {

    Vector3 out;

    out.x = a.m[0][0] * b.x +  a.m[1][0] * b.y +  a.m[2][0] * b.z  +  a.m[3][0] * 1;
    out.y = a.m[0][1] * b.x +  a.m[1][1] * b.y +  a.m[2][1] * b.z  +  a.m[3][1] * 1;
    out.z = a.m[0][2] * b.x +  a.m[1][2] * b.y +  a.m[2][2] * b.z  +  a.m[3][2] * 1;

    return out;
}
