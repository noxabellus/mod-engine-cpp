#include "../../include/audio/lib.hh"
#include "../../include/AssetManager.hh"



namespace mod {
  Audio::Audio (char const* in_origin, f32_t const* in_data, size_t in_data_length, f32_t in_volume)
  : origin(str_clone(in_origin))
  , volume(in_volume)
  { 
    data.append_multiple(in_data, in_data_length);
  }

  Audio Audio::from_ex (char const* origin, f32_t* data, size_t data_length, f32_t volume) {
    Audio audio;

    audio.origin = str_clone(origin);
    audio.data = Array<f32_t>::from_ex(data, data_length);
    audio.volume = volume;

    return audio;
  }

  Audio Audio::from_ex (char const* origin, Array<f32_t> const& data, f32_t volume) {
    Audio audio;

    audio.origin = str_clone(origin);
    audio.data = data;
    audio.volume = volume;

    return audio;
  }

  
  Audio Audio::from_raw_file (char const* origin, char const* source, f32_t volume) {
    if (source == NULL) source = origin;

    using AudioResult = pair_t<f32_t*, size_t>;

    static auto const load_audio_wav = [] (char const* path) -> AudioResult {
      SDL_AudioSpec wav_spec;
      u8_t* wav_start = NULL;
      u32_t wav_length = 0;

      m_asset_assert(
        SDL_LoadWAV(path, &wav_spec, &wav_start, &wav_length) != NULL,
        path,
        "Could not load file"
      );
    
      SDL_AudioCVT cvt;
      SDL_BuildAudioCVT(&cvt, wav_spec.format, wav_spec.channels, wav_spec.freq, AudioContext.audio_spec.format, AudioContext.audio_spec.channels, AudioContext.audio_spec.freq);

      if (!cvt.needed) printf("Warning: applying conversion to wav file that was loaded with the standard spec\n");

      cvt.len = wav_length;
      cvt.buf = memory::allocate<u8_t>(cvt.len * cvt.len_mult);

      memory::copy(cvt.buf, wav_start, wav_length);

      SDL_FreeWAV(wav_start);

      SDL_ConvertAudio(&cvt);

      return { reinterpret_cast<f32_t*>(cvt.buf), static_cast<size_t>(cvt.len_cvt) / sizeof(f32_t) };
    };

    static auto const load_audio_ogg = [] (char const* path) -> AudioResult {
      s32_t err;
      stb_vorbis* file = stb_vorbis_open_filename(path, &err, NULL);

      m_asset_assert(
        file != NULL,
        path,
        "Could not load file, stb error code %d",
        err
      );

      stb_vorbis_info info = stb_vorbis_get_info(file);

      size_t num_floats = stb_vorbis_stream_length_in_samples(file) * info.channels;
      f32_t* imem = memory::allocate<f32_t>(num_floats);

      stb_vorbis_get_samples_float_interleaved(file, info.channels, imem, num_floats);

      stb_vorbis_close(file);

      SDL_AudioCVT cvt;
      SDL_BuildAudioCVT(&cvt, AUDIO_F32SYS, info.channels, info.sample_rate, AudioContext.audio_spec.format, AudioContext.audio_spec.channels, AudioContext.audio_spec.freq);

      if (cvt.needed) {
        cvt.len = num_floats * sizeof(f32_t);
        cvt.buf = memory::allocate<u8_t>(cvt.len * cvt.len_mult);

        memory::copy(cvt.buf, imem, cvt.len);

        memory::deallocate(imem);

        SDL_ConvertAudio(&cvt);

        return { reinterpret_cast<f32_t*>(cvt.buf), static_cast<size_t>(cvt.len_cvt) / sizeof(f32_t) };
      } else {
        return { imem, num_floats };
      }
    };

    using Loader = AudioResult (*) (char const* path);

    static Loader const loaders [2] = { load_audio_wav, load_audio_ogg };

    u8_t format = AudioFormat::from_file_ext(source);

    m_asset_assert(
      format != AudioFormat::Invalid,
      origin,
      "Failed to load Audio source '%s': File extension not recognized. Known file extensions are:\n%s",
      source,
      AudioFormat::known_file_exts
    );

    AudioResult result = loaders[format](source);

    m_asset_assert(
      result.a != NULL && result.b != 0,
      origin,
      "Failed to load Audio source '%s'",
      source
    );

    return Audio { origin, result.a, result.b, volume };
  }

  Audio Audio::from_json_item (char const* origin, JSONItem const& json) {
    JSONItem* path_item = json.get_object_item("path");
    String& path = json.get_object_string("path");

    JSONItem* volume_item = json.get_object_item("volume");

    char relative_path [1024];

    path_item->asset_assert(
      str_dir_relativize_path(origin, path.value, relative_path, 1023),
      "Problem with JSON Audio path: The path given '%s' cannot be properly relativized to the asset path\n"
      "The combined path is either too long, or the asset path is not deep enough to accomodate the back tracking present in the relative image path",
      path.value
    );

    f32_t volume;
    if (volume_item != NULL) {
      json.asset_assert(volume_item->type == JSONType::Number, "Expected a number");
      volume = volume_item->number;
    } else {
      volume = 1.0f;
    }

    return from_raw_file(origin, relative_path, volume);
  }

  Audio Audio::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    Audio audio;

    try {
      audio = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return audio;
  }

  Audio Audio::from_file (char const* origin) {
    for (u8_t format = 0; format < AudioFormat::total_format_count; format ++) {
      if (str_ends_with_caseless(origin, AudioFormat::name(format))) return from_raw_file(origin);
    }

    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load Audio: Unable to read file"
    );

    Audio audio;

    try {
      audio = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      memory::deallocate(source);
      throw exception;
    }

    memory::deallocate(source);

    return audio;
  }


  void Audio::destroy () {
    if (origin != NULL) memory::deallocate(origin);
    data.destroy();
  }
}