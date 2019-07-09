//
// Assets manager
// (c) 2019 Jani Nykänen
//


#ifndef __ASSETS__
#define __ASSETS__

#include "types.h"

#include <stdbool.h>

#define MAX_ASSET_COUNT 256
#define MAX_ASSET_NAME_LENGTH 32

// Asset manager
typedef struct  
{
    
    // Asset names
    // TODO: Check order
    char assetNames [MAX_ASSET_COUNT] [MAX_ASSET_NAME_LENGTH];
    // Asset types (defined in the source)
    int assetTypes [MAX_ASSET_COUNT];
    // Asset pointers
    void* assetPointers [MAX_ASSET_COUNT];
    
    // Asset count
    int assetCount;

} AssetManager;

// Load a bitmap and add it to the
// assets
int assets_add_bitmap(AssetManager* a, const char* name, const char* path, 
    bool linearFilter);

// Load a tilemap and add it to the
// assets
int assets_add_tilemap(AssetManager* a, const char* name, const char* path);

// Get an asset
void* assets_get(AssetManager* a, const char* name);

// Dispose assets
void assets_dispose(AssetManager* a);

// Parse a text file
int assets_parse_text_file(AssetManager* a, const char* path);

// Create an asset manager
AssetManager* create_asset_manager();

#endif // __ASSETS__
