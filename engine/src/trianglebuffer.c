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
    Point A, Point B, Point C, float depth, uint8 col, 
    int dvalue) {

    int i;
    Triangle* next;
    Triangle* prev;

    // Check if room left
    if (buf->triangleCount >= TRIANGLE_BUFFER_SIZE) {

        err_throw_no_param("Triangle buffer overflow!");
        return;
    }
    // Add to the buffer
    Triangle* t = &buf->triangles [buf->triangleCount];
    *t = (Triangle){A, B, C, depth, col, dvalue, NULL};
    
    // Update first
    if (buf->first == NULL || depth > buf->first->depth) {

        t->next = buf->first;
        buf->first = (void*) t;
    }
    else {

        next = buf->first;
        prev = NULL;
        while(true) {

            // If between two elements
            if (prev != NULL &&
                prev->depth >= depth &&
                next->depth < depth) {

                prev->next = (void*)t;
                t->next = (void*)next;
                break;
            }

            prev = next;
            next = (Triangle*)next->next;
            // If end reached, add to the end of
            // the list, kind of
            if (next == NULL) {
                
                t->next = NULL;
                prev->next = (void*)t;
                break;
            }
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
        g_set_darkness(g, g->dvalue);
        g_draw_triangle(g, 
            t->A.x, t->A.y, 
            t->B.x, t->B.y, 
            t->C.x, t->C.y, 
            t->color);
        g_set_darkness(g, 0);

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
