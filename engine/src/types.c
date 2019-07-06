#include "types.h"

#include "math.h"


// Constructors for 2-component containers
Vector2 vec2(float x, float y) {
    return (Vector2){x, y};
}
Point point(int x, int y) {
    return (Point){x, y};
}
