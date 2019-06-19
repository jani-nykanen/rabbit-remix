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


#endif // __TYPES__
