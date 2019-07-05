#include "mesh.h"

#include "err.h"

#include <stdlib.h>


#define DEF_COPY_ARR(type) static type* copy_arr_##type (const type* src, uint32 len) { \
    type* dst = (type*)malloc(sizeof(type)*len);\
    if (dst == NULL) {\
        ERR_MEM_ALLOC;\
        return NULL;\
    }\
    uint32 i;\
    for (i = 0; i < len; ++ i) {\
        dst[i] = src[i];\
    }\
    return dst;\
}\


// Copy array functions
DEF_COPY_ARR(float);
DEF_COPY_ARR(uint16);
DEF_COPY_ARR(uint8);


// Create a mesh
Mesh* create_mesh(
    const float* vertices, 
    const uint8* colors, 
    const float* normals, 
    const uint16* indices,
    uint32 vertexCount, 
    uint32 colorCount, 
    uint32 normalCount,
    uint32 indexCount) {
        
    // Allocate memory
    Mesh* m = (Mesh*)malloc(sizeof(Mesh));
    if (m == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Store sizes
    m->vertexCount = vertexCount;
    m->indexCount = indexCount;
    m->colorCount = colorCount;
    m->normalCount = normalCount;

    // Copy arrays
    m->vertices = copy_arr_float(vertices, vertexCount);
    if (m->vertices == NULL) {
        
        ERR_MEM_ALLOC;
        free(m);
        return NULL;
    }
    m->indices = copy_arr_uint16(indices, indexCount);
    if (m->indices == NULL) {
        
        ERR_MEM_ALLOC;
        free(m);
        return NULL;
    }
    m->normals = copy_arr_float(normals, normalCount);
    if (m->normals == NULL) {
        
        ERR_MEM_ALLOC;
        free(m);
        return NULL;
    }
    m->colors = copy_arr_uint8(colors, colorCount);
    if (m->colors == NULL) {
        
        ERR_MEM_ALLOC;
        free(m);
        return NULL;
    }

    return m;
}


// Destroy a mesh
void destroy_mesh(Mesh* m) {

    if (m == NULL) return;

    free(m->vertices);
    free(m->normals);
    free(m->indices);
    free(m->colors);
    free(m);
}
