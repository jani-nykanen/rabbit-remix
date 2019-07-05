#include "types.h"

#include "math.h"


// Constructors for 2-component containers
Vector2 vec2(float x, float y) {
    return (Vector2){x, y};
}
Point point(int x, int y) {
    return (Point){x, y};
}


// Constructor, 3 components
Vector3 vec3(float x, float y, float z) {

    return (Vector3){x, y, z};
}


// Some operators
Vector3 vec3_subtract(Vector3 a, Vector3 b) {

    return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}
void vec3_normalize(Vector3* v) {

    const float EPS = 0.0001f;

    float len = sqrtf(v->x*v->x + v->y*v->y + v->z*v->z);
    if (len < EPS) {

        v->x = 0;
        v->y = 0;
        v->z = 0;
        return;
    }
    v->x /= len;
    v->y /= len;
    v->z /= len;
}


// Constructor
Vector4 vec4(float x, float y, float z, float w) {

    return (Vector4) {x, y, z, w};
}


// Operators
Vector3 vec4_to_vec3(Vector4 a) {

    return vec3(a.x, a.y, a.z);
}
