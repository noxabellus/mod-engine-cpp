#ifndef APPLICATION_H
#define APPLICATION_H

#include "cstd.hh"
#include "JSON.hh"
#include "Array.hh"
#include "String.hh"
#include "math/Vector2.hh"
#include "Input.hh"



namespace mod {
  struct ResolutionSet {
    using Index = u8_t;
    static constexpr Index max_resolution_count = std::numeric_limits<Index>::max() - 1;
    static constexpr Index null_index = max_resolution_count + 1;

    Vector2s resolutions [max_resolution_count];
    Index count;
    Index active_index;

    Vector2s& operator [] (Index index) const {
      return (Vector2s&) resolutions[index];
    }
  };

  namespace ApplicationWindowMode {
    enum: s8_t {
      Invalid = -1,
      Windowed = 0,
      Fullscreen,
      FullscreenDesktop
    };

    /* Get the name of an ApplicationWindowMode as a str */
    static constexpr char const* name (u8_t mode) {
      switch (mode) {
        case Windowed: return "Windowed";
        case Fullscreen: return "Fullscreen";
        case FullscreenDesktop: return "Fullscreen Desktop";
        default: return "Invalid";
      }
    }

    /* Get an ApplicationWindowMode from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(name, "Windowed", max_length) == 0) return Windowed;
      else if (str_cmp_caseless(name, "Fullscreen", max_length) == 0) return Fullscreen;
      else if (str_cmp_caseless(name, "FullscreenDesktop", max_length) == 0) return FullscreenDesktop;
      else return Invalid;
    }

    /* Convert an ApplicationWindowMode to an SDL enum.
     * Returns SDL_WINDOW_LACKS_SHAPE if the mode was invalid */
    static s32_t to_sdl (u8_t mode) {
      switch (mode) {
        case Windowed: return 0;
        case Fullscreen: return SDL_WINDOW_FULLSCREEN;
        case FullscreenDesktop: return SDL_WINDOW_FULLSCREEN_DESKTOP;
        default: return SDL_WINDOW_LACKS_SHAPE;
      }
    }

    /* Determine if a value is a valid ApplicationWindowMode */
    static constexpr bool validate (s8_t mode) {
      return mode >= Windowed
          && mode <= FullscreenDesktop;
    }
  }

  namespace ApplicationVSyncMode {
    enum: s8_t {
      Invalid = -1,
      None = 0,
      VBlank,
      SleepLock,
      SpinLock
    };
    
    /* Get the name of an ApplicationVSyncMode as a str */
    static constexpr char const* name (u8_t mode) {
      switch (mode) {
        case None: return "None";
        case VBlank: return "VBlank";
        case SleepLock: return "SleepLock";
        case SpinLock: return "SpinLock";
        default: return "Invalid";
      }
    }
    
    /* Get an ApplicationVSyncMode from its name in str form */
    static s8_t from_name (char const* name, size_t max_length = SIZE_MAX) {
      if (str_cmp_caseless(name, "None", max_length) == 0) return None;
      else if (str_cmp_caseless(name, "VBlank", max_length) == 0) return VBlank;
      else if (str_cmp_caseless(name, "SleepLock", max_length) == 0) return SleepLock;
      else if (str_cmp_caseless(name, "SpinLock", max_length) == 0) return SpinLock;
      else return Invalid;
    }
    
    /* Determine if a value is a valid ApplicationVSyncMode */
    static constexpr bool validate (u8_t mode) {
      return mode >= None
          && mode <= SpinLock;
    }
  }

  struct Application_t {
    ENGINE_API static char const* config_path;
    ENGINE_API static char const* imgui_path;
    ENGINE_API static u32_t gl_major_version;
    ENGINE_API static u32_t gl_minor_version;
    ENGINE_API static f32_t max_ui_scale;
    ENGINE_API static f32_t default_font_0_scale;
    ENGINE_API static f32_t default_font_1_scale;
    ENGINE_API static Vector2s min_resolution;
    ENGINE_API static Array<Control> default_controls;


    SDL_Window* window;
    SDL_GLContext gl_context;
    ImGuiContext* ig_context;
    ImGuiIO* ig_io;
    ImGuiStyle* ig_style;
    ImGuiStyle ig_base_style;


    u64_t performance_frequency;

    Vector2s resolution;

    Array<ImFont*> fonts;

    f32_t ui_scale;
    u8_t vsync;
    u8_t target_framerate;
    u8_t window_mode;
    bool show_fps;
    bool show_info;

    u64_t frame_start;

    Input input;


    /* Create the global ModEngine Application, load the config file,
     * and perform supporting library initialization */
    ENGINE_API static Application_t& create ();

    /* Destroy the Application, free allocations, save the config file,
     * and perform supporting library clean up */
    ENGINE_API void destroy ();


    /* Get the set of resolutions supported by the application's current display, as well as the selected resolution */
    ENGINE_API ResolutionSet get_resolution_set () const;

    /* Get the native (vertical blank synchronous) refresh rate of the Application's display */
    ENGINE_API u8_t get_native_framerate () const;


    /* Setup supporting library data and framerate tracking for a new frame */
    ENGINE_API bool begin_frame ();

    /* Finalize supporting library data and framerate tracking for a frame,
     * rendering ui and performing vsync if enabled */
    ENGINE_API void end_frame ();


    /* Set the VSync mode of the Application */
    ENGINE_API void set_vsync (u8_t new_vsync);

    /* Set the resolution of the Application's window (or the display resolution if the Application is in fullscreen mode) */
    ENGINE_API void set_resolution (Vector2s const& new_resolution);

    /* Set the preferred display of the Application by index */
    ENGINE_API void set_display (u8_t new_display_index);

    /* Set the window mode of the Application */
    ENGINE_API void set_window_mode (u8_t new_window_mode);

    /* Set the UI scale of the Application */
    ENGINE_API void set_ui_scale (f32_t new_ui_scale);
  };

  ENGINE_API extern Application_t Application;
}

#endif