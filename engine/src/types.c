#include "types.h"

#include "math.h"


// Constructors for 2-component containers
Vector2 vec2(float x, float y) {
    return (Vector2){x, y};
}
Point point(int x, int y) {
    return (Point){x, y};
}


// Create a 2x2 matrix
Matrix2 mat2(float m11, float m21, float m12, float m22) {

    return (Matrix2){m11, m21, m12, m22};
}


// Compute the inverse matrix
Matrix2 mat2_inverse(Matrix2 A) {

    const float EPS = 0.001f;

    float det = mat2_det(A);
    // If determinant is too close to zero,
    // return an identity matrix
    if (fabsf(det) < EPS) {

        return mat2(1, 0, 0, 1);
    }
    det = 1.0f / det;

    // Compute inverse matrix
    Matrix2 inv;
    inv.m11 = det * (A.m22); inv.m21 = det * -A.m21;
    inv.m12 = det * -(A.m12); inv.m22 = det * A.m11;
    return inv;
}


// Multiply two matrices
Matrix2 mat2_mul(Matrix2 A, Matrix2 B) {

    Matrix2 R;

    R.m11 =  A.m11 * B.m11 + A.m21 * B.m12;
    R.m21 =  A.m11 * B.m21 + A.m21 * B.m22;
    R.m12 =  A.m12 * B.m11 + A.m22 * B.m12;
    R.m22 =  A.m12 * B.m21 + A.m22 * B.m22;

    return R;
}


// Multiply with a vector
Vector2 mat2_mul_vec2(Matrix2 A, Vector2 v) {

    Vector2 r;

    r.x = A.m11 * v.x + A.m21 * v.y;
    r.y = A.m12 * v.x + A.m22 * v.y;

    return r;
}


// Compute determinant
float mat2_det(Matrix2 A) {
    
    return A.m11 * A.m22 - A.m12 * A.m21;
}


// Float matrix to fixed point (=integer) matrix
Mat2Fixed mat2_to_fixed(Matrix2 A) {

    Mat2Fixed R;

    R.m11 = (int)roundf(A.m11 * FIXED_PREC);
    R.m21 = (int)roundf(A.m21 * FIXED_PREC);
    R.m12 = (int)roundf(A.m12 * FIXED_PREC);
    R.m22 = (int)roundf(A.m22 * FIXED_PREC);

    return R;
}


// Multiply fixed point matrix with a point
Point mat2_fixed_mul(Mat2Fixed A, Point p) {

    Point r;

    r.x = (A.m11 * p.x + A.m21 * p.y) / FIXED_PREC;
    r.y = (A.m12 * p.x + A.m22 * p.y) / FIXED_PREC;

    return r;
}
