#include "sample.h"

#include "err.h"
#include "mathext.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


// Load a sample
Sample* load_sample(const char* path) {

    // Allocate memory for a sample
    Sample* s = (Sample*)malloc(sizeof(Sample));
    if (s == NULL) {

        ERR_MEM_ALLOC;
        return NULL;
    }

    // Load audio chunk
    s->chunk = Mix_LoadWAV(path);
    if (s->chunk == NULL) {

        printf("Audio error: %s\n", Mix_GetError());
        err_throw_param_1("Could not load a WAV file in ", path);
        free(s);
        return NULL;
    }

    s->played = false;
    s->channel = -1;

    return s;
}


// Destroy a sample
void sample_destroy(Sample* s) {

    if (s == NULL) return;

    Mix_FreeChunk(s->chunk);
    free(s);
}


// Play sample
void sample_play(Sample* s, float vol, int loops) {

    int v = (int)floorf((float)MIX_MAX_VOLUME * vol);

    if (s->played) {

        Mix_HaltChannel(s->channel);
        Mix_Volume(s->channel, v);
        Mix_PlayChannel(s->channel, s->chunk, loops);
    }
    else {

        s->channel = Mix_PlayChannel(-1, s->chunk, 0);
        Mix_HaltChannel(s->channel);

        Mix_Volume(s->channel, v);
        Mix_PlayChannel(s->channel, s->chunk, loops);

        s->played = true;
    }
}


// Stop sample
void sample_stop(Sample* s) {

    if (!s->played)
        return;

    Mix_HaltChannel(s->channel);
}
