//
// Game scene
// (c) 2019 Jani Nykänen
//

#ifndef __GAME__
#define __GAME__

#include <engine/scene.h>

// Get scene
Scene game_get_scene();

// Draw guide
void game_draw_guide(Graphics* g, bool force);

#endif // __GAME__
