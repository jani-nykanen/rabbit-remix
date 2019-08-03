//
// Audio sample
// (c) 2019 Jani Nykänen
//

#ifndef __SAMPLE__
#define __SAMPLE__

#include <SDL2/SDL_mixer.h>

#include <stdbool.h>

// Sample type
typedef struct {

    Mix_Chunk* chunk;
    int channel;
    bool played;

} Sample;

// Load a sample
Sample* load_sample(const char* path);

// Destroy a sample
void sample_destroy(Sample* s);

// Play sample
void sample_play(Sample* s, float vol, int loops);

// Stop sample
void sample_stop(Sample* s);

#endif // __SAMPLE__
