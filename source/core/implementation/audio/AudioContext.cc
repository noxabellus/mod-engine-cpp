#include "../../include/audio/lib.hh"



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
  }

 
  void AudioContext_t::render_element (AudioState& element) {
    element.render(stream_buffer.elements, buffer_rate);
  }


  void AudioContext_t::queue_buffer () {
    // TODO handle large buildups in queue
    if (last_frame != 0) {
      if (stream_buffer.count > 0) SDL_QueueAudio(device_id, stream_buffer.elements, sizeof(f32_t) * stream_buffer.count);
      
      u64_t frame = SDL_GetPerformanceCounter();
      u64_t frame_delta = frame - last_frame;
      last_frame = frame;

      f64_t frame_delta_s = static_cast<f64_t>(frame_delta) / perf_freq;

      buffer_rate = ceil(buffer_base_rate * frame_delta_s);
    } else {
      last_frame = SDL_GetPerformanceCounter();
      
      buffer_rate = buffer_base_rate / 60.0;
    }

    stream_buffer.reallocate(buffer_rate);
    stream_buffer.count = buffer_rate;
    memory::clear(stream_buffer.elements, buffer_rate);
  }
}