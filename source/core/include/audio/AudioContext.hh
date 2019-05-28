#ifndef AUDIO_CONTEXT_H
#define AUDIO_CONTEXT_H

#include "../cstd.hh"
#include "../Array.hh"



namespace mod {
  struct AudioElement {
    f32_t* data;
    size_t position;
    size_t length;
    f32_t volume;
    bool active;
    bool loop;


    AudioElement () { }
    AudioElement (f32_t* in_data, size_t in_length, f32_t in_volume = 1.0f, bool in_active = true, bool in_loop = true)
    : data(in_data)
    , position(0)
    , length(in_length)
    , volume(in_volume)
    , active(in_active)
    , loop(in_loop)
    { }

    void destroy () {
      if (data != NULL) memory::deallocate(data);
    }


    void add_stream_data (f32_t* stream, size_t stream_length) {
      if (!active) return;

      size_t rem = length - position;
    
      size_t len = stream_length > rem? rem : stream_length;

      if (len > 0) {
        for (size_t i = 0; i < len; i ++) {
          stream[i] += data[position + i] * volume;
        }
        
        position += len;
      } else if (loop) {
        position = 0;
      }
    }
  };
  
  struct AudioContext_t {
    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID device_id;

    Array<f32_t> stream_buffer;
    // Array<f32_t> local_buffer;
    // size_t stream_consumed;
    // size_t local_offset;
    
    u64_t last_frame;
    f64_t perf_freq;

    f64_t buffer_base_rate;
    f64_t buffer_padding_ratio = 1;
    size_t buffer_rate;


    AudioContext_t () { }

    ENGINE_API AudioContext_t& init ();

    ENGINE_API void destroy ();
    

    // ENGINE_API void swap_buffers ();

    ENGINE_API void render_element (AudioElement& element);

    ENGINE_API void queue_buffer ();
  };

  ENGINE_API extern AudioContext_t AudioContext;
}

#endif