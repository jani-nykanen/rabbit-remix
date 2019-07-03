#include "trianglebuffer.h"

#include "err.h"
#include "graphics.h"

#include <stdlib.h>


// Create a triangle buffer
TriangleBuffer create_triangle_buffer() {

    TriangleBuffer buf;
    buf.triangleCount = 0;
    buf.first = NULL;

    return buf;
}


// Add a triangle
void tbuf_add_triangle(TriangleBuffer* buf, 
    Point A, Point B, Point C, float depth, uint8 col) {

    // Check if room left
    if (buf->triangleCount >= TRIANGLE_BUFFER_SIZE) {

        err_throw_no_param("Triangle buffer overflow!");
        return;
    }
    Triangle t = (Triangle){A, B, C, depth, col, NULL};

    // Add to the buffer
    buf->triangles [buf->triangleCount] = t;
    
    // Update first
    if (buf->first == NULL || depth < buf->first->depth) {

        buf->first = (void*) &buf->triangles [buf->triangleCount];
    }

    // Find next
    int i = 1;
    for (; i < buf->triangleCount; ++ i) {

        if (buf->triangles[i].depth >= depth &&
            buf->triangles[i-1].depth < depth) {

            t.next = (void*) &buf->triangles[i];
            buf->triangles[i-1].next = (void*) &buf->triangles [buf->triangleCount];

            break;
        }

    }

    ++ buf->triangleCount;
    
}


// Draw triangles
void tbuf_draw_triangles(TriangleBuffer* buf, void* _g) {

    Graphics* g = (Graphics*)_g;

    Triangle* t = (Triangle*)buf->first;
    if (t == NULL) return;

    while (true) {

        // Draw
        g_draw_triangle(g, 
            t->A.x, t->A.y, 
            t->B.x, t->B.y, 
            t->C.x, t->C.y, 
            t->color);

        if (t->next == NULL)
            break;

        t = (Triangle*)t->next;
    }

    // Clear
    tbuf_clear(buf);
}


// Clear triangle buffer
void tbuf_clear(TriangleBuffer* buf) {

    buf->triangleCount = 0;
    buf->first = NULL;
}
