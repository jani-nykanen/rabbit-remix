#include "types.h"


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


// Constructor
Vector4 vec4(float x, float y, float z, float w) {

    return (Vector4) {x, y, z, w};
}

