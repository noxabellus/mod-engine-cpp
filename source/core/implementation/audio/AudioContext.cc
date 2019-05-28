#include "../../include/audio/AudioContext.hh"

namespace mod {
  AudioContext_t AudioContext;

  AudioContext_t& AudioContext_t::init () {
    audio_spec.format = AUDIO_F32SYS;
    // TODO don't hardcode channels
    audio_spec.freq = 44100;
    audio_spec.channels = 2;
    audio_spec.samples = 0;
    audio_spec.userdata = NULL;
    audio_spec.callback = NULL;
    // audio_spec.samples = 256;
    // audio_spec.userdata = this;
    // audio_spec.callback = [] (void* userdata, u8_t* stream, int stream_length) -> void {
    //   f32_t* stream_flt = reinterpret_cast<f32_t*>(stream);
    //   size_t stream_flt_length = stream_length / sizeof(f32_t);

    //   AudioContext_t& ctx = *reinterpret_cast<AudioContext_t*>(userdata);

    //   size_t consume = num::min(ctx.stream_buffer.count - ctx.stream_consumed, stream_flt_length);
      
    //   memory::copy(stream_flt, ctx.stream_buffer.elements + ctx.stream_consumed, consume);

    //   if (consume < stream_flt_length) {
    //     memory::clear(stream_flt + consume, stream_flt_length - consume);
    //   }

    //   ctx.stream_consumed += consume;
    // };
    
    last_frame = 0;
    perf_freq = SDL_GetPerformanceFrequency();

    buffer_base_rate = ceil(static_cast<f64_t>(audio_spec.freq) * static_cast<f64_t>(audio_spec.channels));
  
    device_id = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
    m_assert(device_id != 0, "Could not open audio device with the designated spec");

    SDL_PauseAudioDevice(device_id, 0);

    return *this;
  }

  void AudioContext_t::destroy () {
    SDL_CloseAudioDevice(device_id);

    stream_buffer.destroy();
    // local_buffer.destroy();
  }

  // void AudioContext_t::swap_buffers () {
  //   static Array<f32_t> intrm_buffer { 0, true };

  //   if (last_frame != 0) {
  //     SDL_LockAudio();

  //     size_t unconsumed = stream_buffer.count - stream_consumed;

  //     intrm_buffer.clear();
  //     intrm_buffer.append_multiple(stream_buffer.elements + stream_consumed, unconsumed);

  //     stream_buffer.clear();
  //     stream_buffer.append_multiple(local_buffer.elements, local_buffer.count);
  //     stream_consumed = 0;

  //     SDL_UnlockAudio();

  //     local_buffer.clear();
  //     local_buffer.append_multiple(intrm_buffer.elements, intrm_buffer.count);

  //     local_offset = unconsumed;

  //     u64_t frame = SDL_GetPerformanceCounter();
  //     u64_t frame_delta = frame - last_frame;
  //     last_frame = frame;

  //     f64_t frame_delta_s = static_cast<f64_t>(frame_delta) / perf_freq;

  //     buffer_rate = ceil(buffer_base_rate * (frame_delta_s * buffer_padding_ratio));

  //     local_buffer.reallocate(buffer_rate);

  //     local_buffer.count = buffer_rate;

  //     if (buffer_rate > local_offset) {
  //       memory::clear(local_buffer.elements + local_offset, buffer_rate - local_offset);
  //     }
  //   } else {
  //     printf("First audio frame\n");
  //     buffer_rate = audio_spec.samples;
      
  //     local_buffer.reallocate(buffer_rate);
  //     local_buffer.count = buffer_rate;
  //     memory::clear(local_buffer.elements, buffer_rate);
  //     last_frame = SDL_GetPerformanceCounter();

  //     SDL_PauseAudioDevice(device_id, 0);
  //   }
  // }

  

  void AudioContext_t::render_element (AudioElement& element) {
    // if (local_offset < buffer_rate) 
    element.add_stream_data(stream_buffer.elements, buffer_rate);
  }


  void AudioContext_t::queue_buffer () {
    if (last_frame != 0) {
      if (stream_buffer.count > 0) SDL_QueueAudio(device_id, stream_buffer.elements, sizeof(f32_t) * stream_buffer.count);
      
      u64_t frame = SDL_GetPerformanceCounter();
      u64_t frame_delta = frame - last_frame;
      last_frame = frame;

      f64_t frame_delta_s = static_cast<f64_t>(frame_delta) / perf_freq;

      buffer_rate = ceil(buffer_base_rate * (frame_delta_s * buffer_padding_ratio));
    } else {
      last_frame = SDL_GetPerformanceCounter();
      
      buffer_rate = buffer_base_rate / 60.0;
    }

    stream_buffer.reallocate(buffer_rate);
    stream_buffer.count = buffer_rate;
    memory::clear(stream_buffer.elements, buffer_rate);
  }
}