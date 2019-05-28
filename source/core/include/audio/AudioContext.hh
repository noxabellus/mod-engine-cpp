#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#include "../cstd.hh"
#include "../Array.hh"



namespace mod {
  struct AudioState;
  
  struct AudioContext_t {
    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID device_id;

    Array<f32_t> stream_buffer;
    
    u64_t last_frame;
    f64_t perf_freq;

    f64_t buffer_base_rate;
    size_t buffer_rate;


    ENGINE_API AudioContext_t& init ();

    ENGINE_API void destroy ();
    

    ENGINE_API void render_element (AudioState& element);

    ENGINE_API void queue_buffer ();
  };

  ENGINE_API extern AudioContext_t AudioContext;
}

#endif