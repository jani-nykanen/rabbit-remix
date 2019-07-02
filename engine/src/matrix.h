//
// Matrices
// (c) 2019 Jani Nykänen
//


#ifndef __MATRIX__
#define __MATRIX__

#include "types.h"

// Matrix type
typedef struct {

    float m[4][4];
} Mat4;

// Create an identity matrix
Mat4 mat4_identity();

// Create a translation matrix
Mat4 mat4_translate(float x, float y, float z);

// Create a scaling matrix
Mat4 mat4_scale(float x, float y, float z);

// Create a rotation matrix
Mat4 mat4_rotate(float angle, float x, float y, float z);

// Multiply
Mat4 mat4_mul(Mat4 a, Mat4 b);

#endif // __MATRIX__
