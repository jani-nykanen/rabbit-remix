#include "transition.h"

#include "eventmanager.h"

#include <stdlib.h>
#include <math.h>

// Initial time
static const float TR_INITIAL_TIME = 60.0f;


// Create a transition object
Transition create_transition_object() {

    Transition tr;
    tr.active = false;

    return tr;
}


// Activate
void tr_activate(Transition* tr, 
    Mode mode, Effect e,
    float speed, void (*cb)(void* e), uint8 c) {

    tr->timer = TR_INITIAL_TIME;
    tr->speed = speed;
    tr->mode = mode;
    
    // Zoom effect is only possible inwards
    if (e == EffectZoom && mode == FadeOut) 
        tr->effect = EffectFade;
    else 
        tr->effect = e;

    tr->callback = cb;
    tr->fadeColor = c;
    

    tr->bufferCopied = false;
    tr->active = true;
}


// Update transition
void tr_update(Transition* tr, void* e, float tm) {

    if (!tr->active) return;

    // Update timer
    tr->timer -= tr->speed * tm;
    if (tr->timer <= 0.0f) {

        if (tr->mode == FadeIn) {

            if (tr->callback != NULL) {

                tr->callback(e);
            }

            tr->timer += TR_INITIAL_TIME;
            tr->mode = FadeOut;

            // Zooming is possible only inwards
            if (tr->effect == EffectZoom) {

                tr->effect = EffectFade;
            }
        }
        else {

            tr->active = false;
            tr->timer = 0.0f;
        }
    }
}


// Draw transition
void tr_draw(Transition* tr, Graphics* g) {

    const float ANGLE_MAX = M_PI / 3.0f;
    const float SCALE = 0.75f;

    if (!tr->active) return;

    float t;
    int dvalue;

    // Copy buffer
    if (tr->effect == EffectZoom && !tr->bufferCopied) {

        g_copy_to_buffer(g);
        tr->bufferCopied = true;
    }

    // Compute fade value
    t = 1.0f / TR_INITIAL_TIME * tr->timer;
    if (tr->mode == FadeIn) {

        t = 1.0f - t;
    }
    
    switch (tr->effect)
    {
    case EffectZoom:

        g_fill_zoomed_rotated(g, &g->bufferCopy, -t * ANGLE_MAX, 
            1.0f-t*SCALE, 1.0f-t*SCALE);

    case EffectFade:
        
        g_set_darkness_color(g, tr->fadeColor);
        dvalue = (int)roundf(t * 14.0f);
        g_darken(g, dvalue);
        break;

    default:
        break;
    }
    
}


// Get timer value in [0,1]
float tr_get_scaled_time(Transition* tr) {

    float t = 1.0f / TR_INITIAL_TIME * tr->timer;
    if (tr->mode == FadeIn) {

        t = 1.0f - t;
    }

    return t;
}
