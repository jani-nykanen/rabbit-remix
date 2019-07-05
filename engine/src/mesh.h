//
// Mesh
// (c) 2019 Jani Nykänen
//

#ifndef __MESH__
#define __MESH__

#include "types.h"

// Mesh type
typedef struct {

    // Counts
    uint32 vertexCount;
    uint32 indexCount;
    uint32 colorCount;
    uint32 normalCount;

    // Data
    float* vertices;
    uint8* colors;
    float* normals;
    uint16* indices;

} Mesh;

// Create a mesh
Mesh* create_mesh(
    const float* vertices, 
    const uint8* colors, 
    const float* normals, 
    const uint16* indices,
    uint32 vertexCount, 
    uint32 colorCount, 
    uint32 normalCount, 
    uint32 indexCount);

// Destroy a mesh
void destroy_mesh(Mesh* m);

#endif // __MESH__
