//
// Useful types
// (c) 2019 Jani Nykänen
//


#ifndef __TYPES__
#define __TYPES__

// Let's put it here
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

#define FIXED_PREC 256

#define KVPAIR_KEY_LENGTH 64
#define KVPAIR_VALUE_LENGTH 256

// Integer types
#define INT_TYPE(name, type) typedef signed type name; typedef unsigned type u##name;
INT_TYPE(int32, int);
INT_TYPE(int64, long);
INT_TYPE(int16, short);
INT_TYPE(int8, char);

// 2-component vector types
typedef struct {

    float x, y;
} Vector2;
typedef struct
{
    int x, y;
} Point;


// Key-value pairs
typedef struct {

    char key [KVPAIR_KEY_LENGTH];
    int value;

} KeyValuePairInt;
typedef struct {

    char key [KVPAIR_KEY_LENGTH];
    char value [KVPAIR_VALUE_LENGTH];

} KeyValuePair;

// Constructors
Vector2 vec2(float x, float y);
Point point(int x, int y);

// 2x2 matrix
typedef struct {

    float m11, m21,
          m12, m22;  

} Matrix2;

// 2x2 matrix with fixed point numbers
typedef struct {

    int m11, m21,
        m12, m22;

} Mat2Fixed;

// Create a 2x2 matrix
Matrix2 mat2(float m11, float m21, float m12, float m22);

// Compute the inverse matrix
Matrix2 mat2_inverse(Matrix2 A);

// Multiply two matrices
Matrix2 mat2_mul(Matrix2 A, Matrix2 B);

// Multiply with a vector
Vector2 mat2_mul_vec2(Matrix2 A, Vector2 v);

// Compute determinant
float mat2_det(Matrix2 A);

// Float matrix to fixed point (=integer) matrix
Mat2Fixed mat2_to_fixed(Matrix2 A);

// Multiply fixed point matrix with a point
Point mat2_fixed_mul(Mat2Fixed A, Point p);

#endif // __TYPES__
