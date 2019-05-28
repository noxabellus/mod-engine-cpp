#ifndef AUDIO_STATE_H
#define AUDIO_STATE_H

#include "Audio.hh"

namespace mod {
  struct AudioState {
    AudioHandle audio;

    size_t position;
    f32_t volume;
    bool active;
    bool loop;


    AudioState () { }

    AudioState (AudioHandle in_audio, f32_t in_volume = 1.0f, bool in_active = true, bool in_loop = true)
    : audio(in_audio)
    , position(0)
    , volume(in_volume)
    , active(in_active)
    , loop(in_loop)
    { }


    ENGINE_API void render (f32_t* stream, size_t stream_length);
  };
}

#endif