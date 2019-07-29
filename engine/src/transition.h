//
// Transition
// (c) 2019 Jani Nykänen
//


#ifndef __TRANSITION__
#define __TRANSITION__

#include  "graphics.h"

#include <stdbool.h>


// Mode
enum {
    FadeIn = 0,
    FadeOut = 1
};
typedef int Mode;

// Effect
enum {
    EffectFade = 0,
    EffectZoom = 1,
    EffectCircle = 2,
};
typedef int Effect;

// Transition type
typedef struct {
    
    Mode mode;
    Effect effect;

    float timer;
    float speed;
    uint8 fadeColor;

    bool active;
    bool bufferCopied;

    void (*callback)(void* e);

} Transition;


// Create a transition object
Transition create_transition_object();

// Activate
void tr_activate(Transition* tr, 
    Mode mode, Effect e,
    float speed, void (*cb)(void* e), uint8 c);

// Update transition
void tr_update(Transition* tr, void* e, float tm);
// Draw transition
void tr_draw(Transition* tr, Graphics* g);

// Get timer value in [0,1]
float tr_get_scaled_time(Transition* tr);

#endif // __TRANSITION__
