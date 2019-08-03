#include "assets.h"

#include "err.h"
#include "bitmap.h"
#include "tilemap.h"
#include "wordreader.h"
#include "sample.h"

#include <stdio.h>
#include <string.h>

// Asset types
enum {

    TypeUnknown = -1,
    TypeBitmap = 0,
    TypeTilemap = 1,
    TypeSample = 2,
    TypeMusic = 3,

    TypeFlag = 4,
};


// Loading functions
static void* cb_load_bitmap(const char* path, bool flag) {

    return (void*) load_bitmap(path, flag);
}
static void* cb_load_tilemap(const char* path, bool flag) {

    return (void*) load_tilemap(path);
}
static void* cb_load_sample(const char* path, bool flag) {

    return (void*) load_sample(path);
}


// Generic loading function
int assets_add_generic(AssetManager* a, 
    void* (*lfunc) (const char*, bool),
    const char* name, const char* path,
    int type, bool flag) {

    if (a->assetCount == MAX_ASSET_COUNT) {

        err_throw_no_param("Asset manager storage is full.");
        return -1;
    }

    // Load
    void* b = lfunc(path, flag);
    if (b == NULL) {

        return -1;
    }

    // Put to the storage
    a->assetTypes[a->assetCount] = type;
    snprintf(a->assetNames[a->assetCount], 
        MAX_ASSET_NAME_LENGTH, "%s", name);
    a->assetPointers[a->assetCount] = (void*)b;
    ++ a->assetCount;

    return 0;
}


// Load a bitmap and add it to the
// assets
int assets_add_bitmap(AssetManager* a, const char* name, const char* path, 
    bool dithering) {

    return assets_add_generic(a, cb_load_bitmap, name, path,
        TypeBitmap, dithering);
}


// Load a tilemap and add it to the
// assets
int assets_add_tilemap(AssetManager* a, const char* name, const char* path) {

    return assets_add_generic(a, cb_load_tilemap, name, path,
        TypeTilemap, false);
}

// Load a tilemap and add it to the
// assets
int assets_add_sample(AssetManager* a, const char* name, const char* path) {

    return assets_add_generic(a, cb_load_sample, name, path,
        TypeSample, false);
}


// Get an asset
void* assets_get(AssetManager* a, const char* name) {

    int i = 0;
    for(; i < a->assetCount; ++ i) {

        if (strcmp(name, a->assetNames[i]) == 0) {

            return a->assetPointers[i];
        }
    }
    return NULL;
}


// Dispose assets
void assets_dispose(AssetManager* a) {

    int i = 0;
    for(; i < a->assetCount; ++ i) {

        switch (a->assetTypes[i])
        {
        case TypeBitmap:
            
            destroy_bitmap((Bitmap*)a->assetPointers[i]);
            break;

        case TypeTilemap:
            
            destroy_tilemap((Tilemap*)a->assetPointers[i]);
            break;

        case TypeSample:
            
            // TODO: Might cause crashes
            sample_destroy((Sample*)a->assetPointers[i]);
            break;    
        
        default:
            break;
        }
    }

    // Destroy the manager itself
    free(a);
}


// Parse a text file
int assets_parse_text_file(AssetManager* a, const char* path) {

    // Create a word reader
    WordReader* wr = create_word_reader(path);
    if (wr == NULL) {

        return -1;
    }

    int type = TypeBitmap;
    int c = 0;

    char name [WR_WORD_LENGTH]; 

    // Render flags
    bool dithering = false;

    while(wr_read_next(wr)) {

        // Type
        if (c == 0) {

            type = TypeUnknown;

            if (strcmp(wr->word, "bitmap") == 0) 
                type = TypeBitmap;
            else if (strcmp(wr->word, "tilemap") == 0) 
                type = TypeTilemap;
            else if (strcmp(wr->word, "sample") == 0) 
                type = TypeSample;    

            else if (strcmp(wr->word, "flag") == 0) 
                type = TypeFlag;
        }
        // Name
        else if (c == 1) {

            snprintf(name, WR_WORD_LENGTH, "%s", wr->word);
        }
        // Path
        else if (c == 2) {

            switch (type)
            {
            // Bitmap
            case TypeBitmap:
                if (assets_add_bitmap(a, name, wr->word, dithering) == -1) {

                    return -1;
                }
                break;

            // Tilemap
            case TypeTilemap:
                if (assets_add_tilemap(a, name, wr->word) == -1) {

                    return -1;
                }
                break;

            // Sample
            case TypeSample:
                if (assets_add_sample(a, name, wr->word) == -1) {

                    return -1;
                }
                break;
            
            // Flag
            case TypeFlag:

                if (strcmp(name, "dither") == 0) {

                    dithering = (int)(strtol(wr->word, NULL, 10)) == 1;
                }

                break;

            default:
                break;
            }
            
        }

        ++ c;
        c %= 3;
    }

    // Close
    dispose_word_reader(wr);

}


// Create an asset manager
AssetManager* create_asset_manager() {

    AssetManager* a = (AssetManager*)malloc(sizeof(AssetManager));
    if (a == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }
    a->assetCount = 0;

    return a;
}
