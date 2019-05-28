#include "../../include/audio/lib.hh"



namespace mod {
  void AudioState::render (f32_t* stream, size_t stream_length) {
    Audio& ref = *audio;

    if (!active) return;

    size_t rem = ref.data.count - position;
  
    size_t len = stream_length > rem? rem : stream_length;

    if (len > 0) {
      for (size_t i = 0; i < len; i ++) {
        stream[i] += ref.data[position + i] * ref.volume * volume;
      }
      
      position += len;
    } else if (loop) {
      position = 0;
    }
  }
}