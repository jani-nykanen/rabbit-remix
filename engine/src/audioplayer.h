//
// Audio player
// (c) 2019 Jani Nykänen
//

#ifndef __AUDIO_PLAYER__
#define __AUDIO_PLAYER__

#include "config.h"
#include "sample.h"

// Audio player type
typedef struct {

    int sfxVolume;
    int musicVolume;

} AudioPlayer;

// Create an audio player
AudioPlayer create_audio_player(Config* conf);

// Change sfx volume
void audio_change_sfx_volume(AudioPlayer* a, int vol);
// Change music volume
void audio_change_music_volume(AudioPlayer* a, int vol);

// Play sample
void audio_play_sample(AudioPlayer* a, Sample* s, float vol, int loops);

// Stop all samples
void audio_stop_samples(AudioPlayer* a);

#endif // __AUDIO_PLAYER__
