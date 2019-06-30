#include "scenemanager.h"

#include "err.h"

#include <stdio.h>
#include <string.h>


// Create a scene manager
SceneManager create_scene_manager() {

    SceneManager s;
    s.active = NULL;
    s.global = NULL;
    s.sceneCount = 0;

    return s;
}


// Add a scene
int scenes_add(SceneManager* sman, 
    const char* name, Scene s, 
    bool makeActive, bool makeGlobal) {
    
    if (sman->sceneCount >= MAX_SCENE_COUNT) {

        err_throw_no_param("Scene manager is full.");
        return -1;
    }

    // Put to the storage
    snprintf(sman->sceneNames[sman->sceneCount], 
        MAX_SCENE_NAME_LENGTH, "%s", name);
    sman->scenes[sman->sceneCount] = s;
    ++ sman->sceneCount;

    // Make active or global
    if (makeActive)
        sman->active = &sman->scenes[sman->sceneCount -1];

    else if (makeGlobal)
        sman->global = &sman->scenes[sman->sceneCount -1];

    return 0;
}


// Change the scene
void scenes_change(SceneManager* sman, const char* name, void* param) {

    int i = 0;
    for(; i < sman->sceneCount; ++ i) {

        if (strcmp(name, sman->sceneNames[i]) == 0) {

            sman->active = &sman->scenes[i];
            if (sman->active->onChange != NULL)
                sman->active->onChange(param);

            return;
        }
    }
}


// Initialize scenes
int scenes_init(SceneManager* sman, void* evMan) {

    int i = 0;
    for(; i < sman->sceneCount; ++ i) {

        if (sman->scenes[i].init != NULL) {

            if (sman->scenes[i].init(evMan) == -1) {

                return -1;
            }
        }
    }
    return 0;
}


// Call "onLoad"
int scenes_on_load(SceneManager* sman, AssetManager* assets) {

    int i = 0;
    for(; i < sman->sceneCount; ++ i) {

        if (sman->scenes[i].onLoad != NULL) {

            if (sman->scenes[i].onLoad(assets) == -1) {

                return -1;
            }
        }
    }
    return 0;
}


// Update active scenes
void scenes_update_active(SceneManager* sman, 
    void* evMan, float tm) {

    // Update active
    if (sman->active != NULL && sman->active->update != NULL)
        sman->active->update((void*)evMan, tm);

    // Update global
    if (sman->active != sman->global && 
        sman->global != NULL && 
        sman->global->update != NULL)
        sman->global->update((void*)evMan, tm);
}


// Draw active scenes
void scenes_draw_active(SceneManager* sman, Graphics* g) {

    // Draw active
    if (sman->active != NULL && sman->active->draw != NULL)
        sman->active->draw(g);

    // Draw global
    if (sman->active != sman->global && 
        sman->global != NULL && 
        sman->global->draw != NULL)
        sman->global->draw(g);
}


// Dispose scenes
void scenes_dispose(SceneManager* sman)  {

    int i = 0;
    for(; i < sman->sceneCount; ++ i) {

        if (sman->scenes[i].dispose != NULL) {

            sman->scenes[i].dispose();
        }
    }
}
