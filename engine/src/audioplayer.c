#include "audioplayer.h"

#include "mathext.h"

#include <math.h>
#include <stdlib.h>

// Create an audio player
AudioPlayer create_audio_player(Config* conf) {

    AudioPlayer a;
    a.musicVolume = conf_get_param_int(conf, "music_volume", 100);
    a.sfxVolume = conf_get_param_int(conf, "sfx_volume", 100);

    return a;
}


// Change sfx volume
void audio_change_sfx_volume(AudioPlayer* a, int vol) {

    a->sfxVolume = min_int32_2(100, max_int32_2(0, vol));
}


// Change music volume
void audio_change_music_volume(AudioPlayer* a, int vol) {

    a->musicVolume = min_int32_2(100, max_int32_2(0, vol));
}


// Play sample
void audio_play_sample(AudioPlayer* a, Sample* s, float vol, int loops) {

    if (a->sfxVolume <= 0) return;

    float v = (a->sfxVolume / 100.0f) * vol;
    sample_play(s, v, loops);
}


// Stop all samples
void audio_stop_samples(AudioPlayer* a) {

    Mix_HaltChannel(-1);
}
