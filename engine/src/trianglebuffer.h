//
// A buffer for triangles
// (c) 2019 Jani Nykänen
//

#ifndef __TRIANGLE_BUFFER__
#define __TRIANGLE_BUFFER__

#include "types.h"

#define TRIANGLE_BUFFER_SIZE 256

// Triangle type
typedef struct {

    // Points
    Point A, B, C;
    // Depth
    float depth;
    // Color
    uint8 color;
    // Darkness
    int dvalue;

    // Next
    void* next;

} Triangle;


// Triangle buffer type
typedef struct
{
    // Triangles
    Triangle triangles [TRIANGLE_BUFFER_SIZE];
    // Count
    int triangleCount;
    // First
    Triangle* first;

} TriangleBuffer;


// Create a triangle buffer
TriangleBuffer create_triangle_buffer();

// Add a triangle
void tbuf_add_triangle(TriangleBuffer* buf, 
    Point A, Point B, Point C, float depth, uint8 col, 
    int dvalue);

// Draw triangles
void tbuf_draw_triangles(TriangleBuffer* buf, void* _g);

// Clear triangle buffer
void tbuf_clear(TriangleBuffer* buf);

#endif // __TRIANGLE_BUFFER__
