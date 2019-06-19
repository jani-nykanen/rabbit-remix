//
// Tilemap
// (c) 2019 Jani Nykänen
//

#ifndef __TILEMAP__
#define __TILEMAP__

// Maximum layer count
#define MAX_LAYER_COUNT 8
// Name length (max)
#define TILEMAP_NAME_LENGTH 128

// Tilemap
typedef struct
{   
    int width, height;
    int* layers [MAX_LAYER_COUNT];
    int layerCount;
    char name[TILEMAP_NAME_LENGTH];
    
} Tilemap;

// Load a tilemap
Tilemap* load_tilemap(const char* path);

// Destroy a tilemap
void destroy_tilemap(Tilemap* t);

#endif // __TILEMAP__
