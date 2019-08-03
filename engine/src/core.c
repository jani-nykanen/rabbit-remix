#include "core.h"

#include "err.h"
#include "mathext.h"

#include <stdlib.h>
#include <stdio.h>

#include <SDL2/SDL_mixer.h>


// Initialize SDL
static int core_init_SDL(Core* c) {

    // Initialize
    if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {

        err_throw_param_1("SDL2 ERROR: ", SDL_GetError());
        return -1;
    }

    // Initialize audio
    if (Mix_OpenAudio(22050, AUDIO_F32, 2, 512) != 0 ||
        Mix_Init(0) != 0) {

        err_throw_param_1("Failed to initialize audio: %s", Mix_GetError());
        return -1;
    }

    // Get window width and height
    int width = conf_get_param_int(&c->conf, "window_width", 640);
    int height = conf_get_param_int(&c->conf, "window_height", 480);
    char* caption = conf_get_param(&c->conf, "window_caption", "Game");

    // Create window
    c->window = SDL_CreateWindow(
            caption, 
            SDL_WINDOWPOS_CENTERED, 
            SDL_WINDOWPOS_CENTERED, 
            width, height, 
            SDL_WINDOW_RESIZABLE);
    if (c->window == NULL) {

        err_throw_param_1("SDL2 ERROR: ", SDL_GetError());
        return -1;
    }

    // Hide cursor
    SDL_ShowCursor(SDL_DISABLE);

    return 0;
}


// Initialize
static int core_init(Core* c) {

    // Initialize SDL2
    if (core_init_SDL(c) == -1) {

        return -1;
    }

    // Init global graphics context
    if (init_graphics_global() == 1) {

        return -1;
    }

    // Create graphics
    c->g = create_graphics(c->window, &c->conf);
    if (c->g == NULL) {

        return -1;
    }

    // Fullscreen
    c->fullscreen = false;
    if (conf_get_param_int(&c->conf, "window_fullscreen", 0) == 1) {

        core_toggle_fullscreen(c);
    }

    // Create components
    c->input = create_input_manager();
    c->vpad = create_gamepad(&c->input);
    c->assets = create_asset_manager();
    c->tr = create_transition_object();
    c->audio = create_audio_player(&c->conf);
    c->evMan = create_event_manager((void*)c,
        &c->input, &c->vpad, &c->sceneMan,
        c->assets, &c->tr, &c->audio);
    if (c->assets == NULL) {

        return -1;
    }

    // Read "gamepad" configuration
    char* kconfPath = conf_get_param(&c->conf, "key_conf_path", NULL);
    if (kconfPath != NULL) {
        
        if (pad_parse_text_file(&c->vpad, kconfPath) == -1) {

            return -1;
        }
    }

    // Get framerate
    c->frameRate = max_int32_2(30, conf_get_param_int(&c->conf, "framerate", 30));

    // Initialize scenes
    if (scenes_init(&c->sceneMan, (void*)&c->evMan) == -1) {

        return -1;
    }

    // Load assets
    // TODO: In another thread?
    char* assetPath = conf_get_param(&c->conf, "asset_path", NULL);
    if (assetPath != NULL) {

        if (assets_parse_text_file(c->assets, assetPath) == -1) {

            return -1;
        }
    }

    // Call "on load"
    if (scenes_on_load(&c->sceneMan, c->assets) == -1) {

        return -1;
    }

    c->running = true;

    return 0;
}


// Update
static void core_update(Core* c, uint32 delta) {

    // Time multiplier
    float tm = (((float)delta)/1000.0f) / (1.0f/60.0f);

    // Update active scenes
    scenes_update_active(&c->sceneMan, (void*)&c->evMan, tm);

    // Update gamepad
    pad_update(&c->vpad);
    // Update input
    input_update(&c->input);

    // Update transition
    tr_update(&c->tr, (void*)&c->evMan, tm);
}


// Draw
static void core_draw(Core* c) {

    // Draw active scenes
    scenes_draw_active(&c->sceneMan, c->g);
    // Draw transition
    tr_draw(&c->tr, c->g);

    // Update canvas
    g_update_pixel_data(c->g);
}


// Handle events
static void core_events(Core* c) {

    SDL_Event e;
    while(SDL_PollEvent(&e) != 0) {

        switch(e.type)
        {
        // Application quit
        case SDL_QUIT:

            core_terminate(c);
            return;

        // Key down event
        case SDL_KEYDOWN:

            event_key_down(&c->input, (int)e.key.keysym.scancode);
            break;

        // Key up event
        case SDL_KEYUP:

            event_key_up(&c->input, (int)e.key.keysym.scancode);
            break;

        // Window event (resize etc)
        case SDL_WINDOWEVENT:
            // Resize
            if (e.window.windowID == SDL_GetWindowID(c->window) 
                && e.window.event == SDL_WINDOWEVENT_RESIZED) {
                
                   // Resize event for graphics
                   g_resize(c->g, e.window.data1, e.window.data2);
            }
            break;

        // TODO: JOYSTICK!

        // Joystick button pressed
        case SDL_JOYBUTTONDOWN:

            event_joy_down(&c->input, e.jbutton.button);
            break;

            // Joystick button released
            case SDL_JOYBUTTONUP:

                event_joy_up(&c->input, e.jbutton.button);
                break;

            // Joystick motion
            case SDL_JOYAXISMOTION: 
            {
                    int axis = 0;
                    if (e.jaxis.axis == 0)
                        axis = 0;

                    else if (e.jaxis.axis == 1)
                        axis = 1;
                    else 
                        break;

                    float value = (float)e.jaxis.value / 32767.0f;
                    event_joy_move(&c->input, value, axis);
            }
            break;

            // Joystick hat
            case SDL_JOYHATMOTION:
            {
                int v = e.jhat.value;
                Vector2 stick = vec2(0, 0);

                // Check all the possible cases
                if (v == SDL_HAT_LEFTUP || v == SDL_HAT_LEFT 
                || v == SDL_HAT_LEFTDOWN) {

                    stick.x = -1.0f;
                }
                else if (v == SDL_HAT_RIGHTUP || v == SDL_HAT_RIGHT 
                || v == SDL_HAT_RIGHTDOWN){

                    stick.x = 1.0f;
                }

                if (v == SDL_HAT_LEFTUP || v == SDL_HAT_UP 
                || v == SDL_HAT_RIGHTUP) {

                    stick.y = -1.0f;
                }
                else if (v == SDL_HAT_LEFTDOWN || v == SDL_HAT_DOWN 
                || v == SDL_HAT_RIGHTDOWN) {

                    stick.y = 1.0f;
                }

                // Joystick event
                event_joy_move_axes(&c->input, stick.x, stick.y);
              
                break;
            }
    
        default:
            break;
        }
    }
}


// Main loop
static void core_loop(Core* c) {

    const int MAX_UPDATE_COUNT = 5;
    const int COMPARED_FPS = 60;

    // Get framerate for the game logic
    int frameWait = 1000 / c->frameRate;

    // Time
    int timeSum = 0;
    uint32 oldTime = SDL_GetTicks();
    uint32 newTime = oldTime;

    int updateCount = 0;
    bool redraw = false;
    while(c->running) {

        // Framerate may be changed run time
        frameWait = 1000 / c->frameRate;

        // Update events
        core_events(c);

        // Check time
        oldTime = newTime;
        newTime = SDL_GetTicks();
        timeSum += (int)(newTime - oldTime);
        updateCount = 0;

        // Check if enough time passed
        while(timeSum >= frameWait) {

            redraw = true;

            // Update frame
            core_update(c, frameWait);

            // Make sure we won't be updating the frame
            // too many times
            if (++ updateCount >= MAX_UPDATE_COUNT) {

                timeSum = 0;
                break;
            }

            // Reduce time sum
            timeSum -= frameWait; 
        }

        if (redraw) {

            // Draw
            core_draw(c);
            redraw = false;
        }

        // Refresh frame (and draw the canvas)
        g_refresh(c->g);
    }
}


// Destroy
static void core_destroy(Core* c) {

    // Destroy global data
    destroy_global_graphics();

    // Destroy components
    assets_dispose(c->assets);
    dispose_graphics(c->g);

    // Dispose scenes
    scenes_dispose(&c->sceneMan, (void*)&c->evMan);

    // Destroy window
    SDL_DestroyWindow(c->window);
}


// Run
void core_run(Core* c) {

    // Initialize
    if (core_init(c) == -1) {

        printf("Fatal error: %s.\n", get_error());
    }

    // Enter the main loop
    core_loop(c);

    // Destroy
    core_destroy(c);
}


// Toggle fullscreen
void core_toggle_fullscreen(Core* c) {

    c->fullscreen = !c->fullscreen;
    SDL_SetWindowFullscreen(c->window,
        c->fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
}


// Terminate
void core_terminate(Core* c) {

    c->running = false;
}


// Create a core
Core* create_core() {

    // Allocate memory
    Core* c = (Core*)malloc(sizeof(Core));
    if (c == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Create configuration
    c->conf = create_config();
    // Create scene manager
    c->sceneMan = create_scene_manager();

    return c;
}
