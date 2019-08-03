//
// Scene manager
// (c) 2019 Jani Nykänen
//


#ifndef __SCENE_MANAGER__
#define __SCENE_MANAGER__

#include "scene.h"

#include "assets.h"

#include "stdbool.h"

// Maximum scenes & scene name length
#define MAX_SCENE_COUNT 32
#define MAX_SCENE_NAME_LENGTH 16


// Scene manager type
typedef struct {

    Scene scenes [MAX_SCENE_COUNT];
    char sceneNames [MAX_SCENE_COUNT] [MAX_SCENE_NAME_LENGTH];
    Scene* active;
    Scene* global;
    int sceneCount;

} SceneManager;

// Create a scene manager
SceneManager create_scene_manager();

// Add a scene
int scenes_add(SceneManager* sman, 
    const char* name, Scene s, 
    bool makeActive, bool makeGlobal);

// Change the scene
void scenes_change(SceneManager* sman, 
    const char* name, void* param);

// Initialize scenes
int scenes_init(SceneManager* sman, void* evMan);
// Call "onLoad"
int scenes_on_load(SceneManager* sman, AssetManager* assets);
// Update active scenes
void scenes_update_active(SceneManager* sman, void* evMan, float tm);
// Draw active scenes
void scenes_draw_active(SceneManager* sman, Graphics* g);
// Dispose scenes
void scenes_dispose(SceneManager* sman, void* evMan);

#endif // __SCENE_MANAGER__
