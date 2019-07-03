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
} Matrix4;

// Create a matrix full of zeroes
Matrix4 mat4_zeroes();

// Create an identity matrix
Matrix4 mat4_identity();

// Create a translation matrix
Matrix4 mat4_translate(float x, float y, float z);

// Create a scaling matrix
Matrix4 mat4_scale(float x, float y, float z);

// Create a rotation matrix
Matrix4 mat4_rotate(float angle, float x, float y, float z);

// Create a perspective matrix
Matrix4 mat4_perspective(float fovY, float aspect, float near, float far);

// Multiply
Matrix4 mat4_mul(Matrix4 a, Matrix4 b);

// Multiply with a vector
Vector3 mat4_mul_vec3(Matrix4 a, Vector3 b);

#endif // __MATRIX__
