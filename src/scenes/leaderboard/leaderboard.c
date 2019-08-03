#include "leaderboard.h"

#include <engine/mathext.h>
#include <engine/eventmanager.h>
#include <leaderboard/leaderboard.h>

#include <math.h>
#include <stdlib.h>

// Bitmaps
static Bitmap* bmpFont;

// Was the previous scene the title scene
static bool prevSceneTitle;
// Is the buffer copied
static bool bufferCopied;

// Leaderboard data
static Leaderboard lb;
// Is leaderboard enabled
static bool enabled;

// Threading stuff
static SDL_Thread* thread;
static SDL_mutex* mutex;
static bool dataFetched;
static int status;
static bool ready;

// Connecting animation timer
static float connectAnimTime;


// Fetch data thread
static int thread_fetch_data(void* ptr) {

    Entry* e = (Entry*)ptr;

    if (ptr != NULL) {

        status = lb_add_score(&lb, e->name, e->score);
    }
    else
        status = lb_fetch_scores(&lb);

    dataFetched = true;

    return 0;
}


// Fetch data
static void fetch_data(Entry* entry) {

    if (!enabled) {

        ready = true;
        status = 1;
        return;
    }

    ready = false;
    dataFetched = false;

    // Create a thread
    // TODO: Copy entry to a local object
    thread = SDL_CreateThread(thread_fetch_data, "thread_fetch", entry);
}


// Go to the previous scene
static void go_back(void* e) {

    ev_change_scene((EventManager*)e, 
        prevSceneTitle ? "title" : "game", 
        NULL);
}


// Initialize
static int lboard_init(void* e) {

    // Initialize CURL
    enabled = init_global_leaderboard() == 0;
    if (!enabled) {

        printf("Leaderboard disabled.\n");
    }

    // Create leaderboard
    lb = create_leaderboard();

    // Create mutex
    mutex = SDL_CreateMutex();

    return 0;
}


// On load
static int lboard_on_load(AssetManager* a) { 

    // Get bitmaps
    bmpFont = (Bitmap*)assets_get(a, "font");

    // Set initials
    prevSceneTitle = false;
    bufferCopied = false;
    dataFetched = false;
    connectAnimTime = 0.0f;
}


// Update
static void lboard_update(void* e, float tm) {

    const float CONNECT_ANIM_SPEED = 1.0f / 15.0f;

    EventManager* evMan = (EventManager*)e;

    if (evMan->tr->active) return;

    // Finished?
    if (!ready) {

        // Update animation timer
        connectAnimTime += CONNECT_ANIM_SPEED * tm;
        connectAnimTime = fmodf(connectAnimTime, 4.0f);

        // Check if ready
        if (SDL_LockMutex(mutex) == 0) {

            ready = dataFetched;
            SDL_UnlockMutex(mutex);
        }
    }

    // Wait for enter or fire1 or escape
    if ((ready && (pad_get_button_state(evMan->vpad, "fire1")  == StatePressed ||
        pad_get_button_state(evMan->vpad, "start") == StatePressed)) ||
        pad_get_button_state(evMan->vpad, "cancel")  == StatePressed) {

        if (prevSceneTitle) {

            go_back((void*)evMan);
        }
        else {

            tr_activate(evMan->tr, FadeIn, EffectCircle, 2.0f,
                go_back, 0);
        }

        if (thread != NULL)
            SDL_DetachThread(thread);
    }
}


// Draw
static void lboard_draw(Graphics* g) {

    const int TITLE_Y = 24;
    const int ENTRY_NAME_X = 32;
    const int ENTRY_SCORE_X = g->csize.x/2 + 40;
    const int ENTRY_Y = 40;
    const int ENTRY_OFFSET = 12;
    const int DVALUE = 5;

    const int BOX_OFF_X = 4;
    const int BOX_OFF_Y = 4;

    const uint8 BOX_COLOR[] = {
        255, 0, 83
    };
    const int BOX_OUTLINES = 3;

    const char* CONNECT_STR[] = {
        "CONNECTING   ",
        "CONNECTING.  ",
        "CONNECTING.. ",
        "CONNECTING...",
    };

    int i;
    char buf [LB_NAME_LENGTH +3];

    // Copy buffer & draw it
    if (!bufferCopied) {

        g_copy_to_buffer(g);
        bufferCopied = true;
    }
    g_draw_bitmap(g, &g->bufferCopy, 0, 0, false);
    g_set_darkness_color(g, 0);
    g_darken(g, DVALUE);

    // Draw box
    for (i = 0; i < BOX_OUTLINES; ++ i) {

        g_fill_rect(g, 
            ENTRY_NAME_X-BOX_OFF_X - (BOX_OUTLINES-1 -i), 
            TITLE_Y-BOX_OFF_Y - (BOX_OUTLINES-1 -i), 
            g->csize.x - (ENTRY_NAME_X-BOX_OFF_X)*2 + (BOX_OUTLINES-1 -i)*2, 
            g->csize.y - (TITLE_Y-BOX_OFF_Y)*2 + (BOX_OUTLINES-1 -i)*2, 
            BOX_COLOR[i]);
    }

    // Draw title
    g_draw_text(g, bmpFont, "LEADERBOARD", 
        g->csize.x/2, TITLE_Y, 0, 0, true);

    if (ready) {

        // Error
        if (status != 0) {

            g_draw_text(g, bmpFont, "CONNECTION ERROR.", 
                g->csize.x/2, g->csize.y/2 - 4, 0, 0, true);
        }
        else {

            // Draw entries
            for (i = 0; i < LB_ENTRY_MAX; ++ i) {

                // Draw rank & name
                snprintf(buf, LB_NAME_LENGTH+3, "%d. %s", i+1, lb.entries[i].name);
                g_draw_text(g, bmpFont, buf, 
                    ENTRY_NAME_X, ENTRY_Y + ENTRY_OFFSET*i, 
                    0, 0, false);

                // Draw score
                snprintf(buf, LB_NAME_LENGTH+3, "%d", lb.entries[i].score);
                g_draw_text(g, bmpFont, buf, 
                    ENTRY_SCORE_X, ENTRY_Y + ENTRY_OFFSET*i, 
                    0, 0, false);
            }

        }

    }
    else {

        g_draw_text(g, bmpFont, CONNECT_STR[(int)connectAnimTime], 
            g->csize.x/2, g->csize.y/2 - 4, 0, 0, true);
    }
}


// Dispose
static void lboard_dispose(void* e) {


}


// Change
static void lboard_on_change(void* param) {

    prevSceneTitle = param == NULL;
    bufferCopied = false;

    fetch_data((Entry*)param);
}


// Get the leaderboard scene
Scene lboard_get_scene() {

    Scene s = (Scene) {
        lboard_init,
        lboard_on_load,
        lboard_update,
        lboard_draw,
        lboard_dispose,
        lboard_on_change
    };

    return s;
}
