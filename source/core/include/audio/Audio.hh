#ifndef AUDIO_H
#define AUDIO_H

#include "../cstd.hh"
#include "../util.hh"
#include "../JSON.hh"
#include "../Array.hh"
#include "../Exception.hh"

#include "../AssetHandle.hh"



namespace mod {
  namespace AudioFormat {
    enum: u8_t {
      Wav,
      Ogg,

      total_format_count,

      Invalid = -1
    };

    static constexpr char const* names [total_format_count] = {
      "Wav",
      "Ogg"
    };

    /* Get the name of an AudioFormat as a str */
    static constexpr char const* name (u8_t format) {
      if (format < total_format_count) return names[format];
      else return "Invalid";
    }

    /* Get an AudioFormat from its name in str form */
    static constexpr u8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      for (u8_t format = 0; format < total_format_count; format ++) {
        if ((max_length == SIZE_MAX || strlen(names[format]) == max_length) && str_cmp_caseless(name, names[format], max_length) == 0) return format;
      }

      return Invalid;
    }

    static constexpr char const* extensions [total_format_count] = {
      "wav",
      "ogg"
    };

    /* Get an AudioFormat from a file extension.
     * Accepts whole path or extension only */
    static constexpr u8_t from_file_ext (char const* path, size_t max_length = SIZE_MAX) {
      s64_t ext_offset = str_file_extension(path, max_length);

      char const* offset_path = ext_offset == -1? path : path + ext_offset + 1;
      size_t offset_max = ext_offset == -1? max_length : max_length - ext_offset - 1;

      for (u8_t format = 0; format < total_format_count; format ++) {
        if ((max_length == SIZE_MAX || strlen(names[format]) == max_length) && str_cmp_caseless(offset_path, extensions[format], offset_max) == 0) return format;
      }

      return Invalid;
    }

    /* Get a file extension from an AudioFormat.
     * Returns NULL if the AudioFormat was invalid */
    static constexpr char const* to_file_ext (u8_t format) {
      if (format < total_format_count) return extensions[format];
      else return NULL;
    }

    static constexpr char const* known_file_exts = (
      "Windows Audio (Uncompressed): .wav\n"
      "Vorbis (Compressed): .ogg"
    );

    /* Determine if a value is a valid AudioFormat */
    static constexpr bool validate (u8_t format) {
      return format < total_format_count;
    }
  }

  struct Audio {
    char* origin;
    u32_t asset_id = 0;

    Array<f32_t> data;
    f32_t volume;
    // TODO pitch


    /* Create a new uninitialized Audio asset */
    Audio () { }

    /* Create a new Audio asset and initialize its data by copying from an existing buffer, and optionally give it a volume modifier (Defaults to 1.0f) */
    ENGINE_API Audio (char const* in_origin, f32_t const* in_data, size_t in_data_length, f32_t in_volume = 1.0f);

    /* Create a new Audio asset and initialize its data by copying from an existing array, and optionally give it a volume modifier (Defaults to 1.0f) */
    Audio (char const* in_origin, Array<f32_t> const& in_data, f32_t in_volume = 1.0f)
    : Audio (in_origin, in_data.elements, in_data.count, in_volume)
    { }

    /* Create a new Audio asset and initialize its data by taking ownership of an existing buffer, and optionally give it a volume modifier (Defaults to 1.0f) */
    ENGINE_API static Audio from_ex (char const* origin, f32_t* data, size_t data_length, f32_t volume = 1.0f);

    /* Create a new Audio asset and initialize its data by taking ownership of an existing array, and optionally give it a volume modifier (Defaults to 1.0f) */
    ENGINE_API static Audio from_ex (char const* origin, Array<f32_t> const& data, f32_t volume = 1.0f);


    /* Create a new Audio asset from a raw source file.
     * Optional source parameter defaults to origin if NULL or nothing is passed.
     * This allows a different origin from the source when called by sub-loaders.
     * Optional volume parameter defaults to 1.0f */
    ENGINE_API static Audio from_raw_file (char const* origin, char const* source = NULL, f32_t volume = 1.0f);


    /* Create a new Audio asset from a JSONItem */
    ENGINE_API static Audio from_json_item (char const* origin, JSONItem const& json);

    /* Create a new Audio asset from JSON */
    ENGINE_API static Audio from_json (char const* origin, JSON const& json) {
      return from_json_item(origin, json.data);
    }

    /* Create a new Audio asset from a source str */
    ENGINE_API static Audio from_str (char const* origin, char const* source);

    /* Create a new Audio asset from a source asset file */
    ENGINE_API static Audio from_file (char const* origin);


    /* Clean up an Audio asset's heap allocations */
    ENGINE_API void destroy ();



    /* Throw an exception using the origin of this Asset, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset */
    template <typename ... A> NORETURN void asset_error_terminal (s32_t line, char const* fmt, A ... args) {
      char* e_origin = str_clone(origin);
      destroy();
      m_asset_error_ex(e_origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, char const* fmt, A ... args) {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, and destroy the asset, if a condition is not met */
    template <typename ... A> void asset_assert_terminal (bool cond, s32_t line, char const* fmt, A ... args) {
      if (!cond) asset_error(line, fmt, args...);
    }


    /* Throw an exception using the origin of this Asset */
    template <typename ... A> NORETURN void asset_error (char const* fmt, A ... args) const {
      m_asset_error(origin, -1, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number */
    template <typename ... A> NORETURN void asset_error (s32_t line, char const* fmt, A ... args) const {
      m_asset_error(origin, line, -1, fmt, args...);
    }

    /* Throw an exception using the origin of this Asset, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, char const* fmt, A ... args) const {
      if (!cond) asset_error(fmt, args...);
    }

    /* Throw an exception using the origin of this Asset and a line number, if a condition is not met */
    template <typename ... A> void asset_assert (bool cond, s32_t line, char const* fmt, A ... args) const {
      if (!cond) asset_error(line, fmt, args...);
    }
  };

  using AudioHandle = AssetHandle<Audio>;
}

#endif