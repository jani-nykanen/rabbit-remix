#include "transition.h"

#include <stdlib.h>

// Initial time
static const float TR_INITIAL_TIME = 60.0f;


// Create a transition object
Transition create_transition_object() {

    Transition tr;
    tr.active = false;

    return tr;
}


// Activate
void tr_activate(Transition* tr, Mode mode, 
    float speed, void (*cb)(void), Color c) {

    tr->timer = TR_INITIAL_TIME;
    tr->speed = speed;
    tr->mode = mode;
    tr->callback = cb;
    tr->fadeColor = c;

    tr->active = true;
}


// Update transition
void tr_update(Transition* tr, float tm) {

    if (!tr->active) return;

    // Update timer
    tr->timer -= tr->speed * tm;
    if (tr->timer <= 0.0f) {

        if (tr->mode == FadeIn) {

            if (tr->callback != NULL) {

                tr->callback();
            }

            tr->timer += TR_INITIAL_TIME;
            tr->mode = FadeOut;
        }
        else {

            tr->active = false;
            tr->timer = 0.0f;
        }
    }
}


// Draw transition
void tr_draw(Transition* tr, Graphics* g) {

    if (!tr->active) return;

    // Compute fade value
    float t = 1.0f / TR_INITIAL_TIME * tr->timer;
    if (tr->mode == FadeIn) {

        t = 1.0f - t;
    }
    
    // TODO
}
