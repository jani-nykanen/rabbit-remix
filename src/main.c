//
// Main file
// (c) 2019 Jani Nykänen
//

#include <engine/core.h>
#include <engine/err.h>

#include "scenes/global.h"
#include "scenes/game/game.h"


// Main function
int main(int argc, char** argv) {

    // Create core
    Core* c = create_core();
    if (c == NULL) return 1;

    // Parse configuration file
    if (conf_parse_text_file(&c->conf, "game.conf")) {

        printf("Error: %s\n", get_error());
    }

    // Add scenes
    scenes_add(&c->sceneMan, "global", 
        global_get_scene(), false, true);
    scenes_add(&c->sceneMan, "game", 
        game_get_scene(), true, false);

    // Run
    core_run(c);
}
