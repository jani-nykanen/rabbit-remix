#include "tilemap.h"

#include "err.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Assume we don't want to find words longer than this
#define TEMP_BUFFER_SIZE 128



// Find a word location
static int find_word(FILE* f, const char* find) {

    int len = strlen(find);
    int bufferPointer =0;
    int pos = (int)ftell(f);
    
    char c;
    while((c = fgetc(f)) != EOF) {

        if (c == find[bufferPointer]) {

            ++ bufferPointer;
            if (bufferPointer == len) {

                return pos-len;
            }
        }
        else {

            bufferPointer = 0;
        }

        ++ pos;
    }
    

    return -1;
}


// Get attribute
static void get_attribute(FILE* f, int off, char* buf) {

    int pos = 0;
    char c;
    int wait = off;

    while((c = fgetc(f)) != EOF) {

        if ((-- wait) > 0) continue;

        if (c == '"') {

            buf[pos ++] = '\0';
            break;
        }
        else {
            buf[pos ++] = c;
        }
    }
}


// Parse CSV
static void parse_csv(FILE* f, int off, char terminate, int* buffer) {

    int wait = off;
    char cbuf[9];
    char c;
    int pointer = 0;
    int len = 0;
    while((c = fgetc(f)) != EOF) {

        if ((-- wait) > 0) continue;

        // Store value
        if (c == ',' && len > 0) {

            cbuf[len] = '\0';
            buffer[pointer ++] = (int)strtol(cbuf, NULL, 10);
            len = 0;
        }
        // Ending character reached
        else if (c == terminate) {

            if (len > 0)
                buffer[pointer] = (int)strtol(cbuf, NULL, 10);

            break;
        }
        // Number character
        else if (c >= '0' && c <= '9') {

            cbuf[len ++] = c;
        }
    }
}


// Load a tilemap
Tilemap* load_tilemap(const char* path) {

    // Allocate memory
    Tilemap* t = (Tilemap*)malloc(sizeof(Tilemap));
    if (t == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Open file
    FILE* f = fopen(path, "r");
    if (f == NULL) {

        err_throw_param_1("Failed to open a file in ", path);
        free(t);
        return NULL;
    }

    // Get width
    int p = find_word(f, "width");
    if (p == -1) {

        err_throw_param_1("Could not find a 'width' param in ", path);
        free(t);
        return NULL;
    }
    char buffer [TEMP_BUFFER_SIZE];
    get_attribute(f, 3, buffer);
    t->width = (int)strtol(buffer, NULL, 10);

    // Get height
    p = find_word(f, "height");
    if (p == -1) {

        err_throw_param_1("Could not find a 'height' param in ", path);
        free(t);
        return NULL;
    }
    get_attribute(f, 3, buffer);
    t->height = (int)strtol(buffer, NULL, 10);

    // Find layers
    t->layerCount = 0;
    int i;
    while(t->layerCount < MAX_LAYER_COUNT &&
            find_word(f, "<data encoding=\"csv\">") != -1) {

        // Allocate memory for the new layer
        t->layers[t->layerCount] = (int*)malloc(sizeof(int)*t->width*t->height);
        if (t->layers[t->layerCount] == NULL) {

            ERR_MEM_ALLOC;
            for(i = 0; i < t->layerCount-1; ++ i)
                free(t->layers[i]);
            free(t);
            return NULL;
        }
        // Parse
        parse_csv(f, 1, '<', t->layers[t->layerCount]);

        ++ t->layerCount;

    }

    // Check if a name parameter is given
    rewind(f);
    p = find_word(f, "name=\"name\" value");
    t->name[0] = '\0';
    if (p != -1) {

        get_attribute(f, 3, t->name);
    }

    // Close
    fclose(f);

    return t;
}


// Destroy a tilemap
void destroy_tilemap(Tilemap* t) {

    if (t == NULL) return;

    // Free layers
    int i = 0;
    for(i = 0; i < t->layerCount; ++ i) {

        free(t->layers[i]);
    }
    free(t);
}
