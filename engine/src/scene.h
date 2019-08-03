//
// Scene 
// (c) 2019 Jani Nykänen
//


#ifndef __SCENE__
#define __SCENE__

#include "assets.h"
#include "graphics.h"

// Scene type
typedef struct {

    int (*init)(void*);
    int (*onLoad)(AssetManager*);
    void (*update)(void*, float);
    void (*draw)(Graphics*);
    void (*dispose)(void*);
    void (*onChange)(void*);

} Scene;


#endif // __SCENE__
