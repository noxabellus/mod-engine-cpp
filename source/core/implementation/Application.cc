#include "../include/Application.hh"



namespace mod {
  char const* Application_t::config_path = "./assets/application_config.json";
  char const* Application_t::imgui_path = NULL;
  u32_t Application_t::gl_major_version = 4;
  u32_t Application_t::gl_minor_version = 3;
  f32_t Application_t::max_ui_scale = 2.0f;
  f32_t Application_t::default_font_0_scale = 26.0f;
  f32_t Application_t::default_font_1_scale = 13.0f;
  Vector2s Application_t::min_resolution = { 640, 480 };

  Application_t Application = { };

  

  static ImFontConfig DEFAULT_FONT_CONFIG = { };

  #ifndef GL_ERROR_HANDLER
    static void APIENTRY STANDARD_GL_ERROR_HANDLER (
      GLenum source,
      GLenum type,
      GLuint id,
      GLenum severity,
      GLsizei length,
      const GLchar* message,
      const void* user_param
    ) {
      printf("OpenGL Error detected\n");

      printf("Type: ");
      switch (type) {
        case GL_DEBUG_TYPE_ERROR: printf("General\n"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: printf("Deprecated behavior\n"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: printf("Undefined behavior\n"); break;
        case GL_DEBUG_TYPE_PORTABILITY: printf("Portability\n"); break;
        case GL_DEBUG_TYPE_PERFORMANCE: printf("Performance\n"); break;
        default: printf("Other\n");
      }

      printf("ID: %" PRIu32, id);

      printf("Severity: ");
      switch (severity) {
        case GL_DEBUG_SEVERITY_LOW: printf("Low\n"); break;
        case GL_DEBUG_SEVERITY_MEDIUM: printf("Medium\n"); break;
        case GL_DEBUG_SEVERITY_HIGH: printf("High\n"); break;
      }

      printf("Message:\n%s\n", message);

      abort();
    }

    #define gl_error_handler STANDARD_GL_ERROR_HANDLER
  #endif

  Application_t& Application_t::create () {
    m_assert(SDL_Init(SDL_INIT_EVERYTHING) == 0, "Failed to initialize SDL");

    m_assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0, "Failed to set OpenGL Context Profile Mask");
    m_assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version) == 0, "Failed to set OpenGL Major Version %u", gl_major_version);
    m_assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version) == 0, "Failed to set OpenGL Minor Version %u", gl_minor_version);

    // TODO double buffer should be a setting, depth size and stencil size should be statics
    m_assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0, "Failed to enable double buffering");
    m_assert(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24) == 0, "Failed to set depth bpp");
    m_assert(SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8) == 0, "Failed to set stencil size");

    Application.window = SDL_CreateWindow(
      // TODO application name should be customizable somewhere
      "ModEngine",
      SDL_WINDOWPOS_CENTERED_MASK,
      SDL_WINDOWPOS_CENTERED_MASK,
      min_resolution.x,
      min_resolution.y,
      SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_HIDDEN
    );

    m_assert(Application.window != NULL, "Failed to create Application Window");

    Application.gl_context = SDL_GL_CreateContext(Application.window);

    m_assert(Application.gl_context != NULL, "Failed to create Application OpenGL Context");

    m_assert(gl3wInit() == GL3W_OK, "Failed to initialize OpenGL");

    m_assert(gl3wIsSupported(gl_major_version, gl_minor_version), "Unsupported OpenGL Version %u.%u", gl_major_version, gl_minor_version);

    m_assert(IMGUI_CHECKVERSION(), "ImGui version check failed");

    if (glDebugMessageCallback && gl_error_handler) {
      glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
      glDebugMessageCallback(gl_error_handler, NULL);
      uint32_t unused_ids = 0;
      glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unused_ids, true);
    } else {
      printf("Warning: glDebugMessageCallback is not available, OpenGL errors will be uncaught");
    }

    Application.ig_context = ImGui::CreateContext();
    Application.ig_io = &ImGui::GetIO();
    Application.ig_io->IniFilename = imgui_path;
    Application.ig_style = &ImGui::GetStyle();
    Application.ig_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // TODO this should be a config property
    ImGui::StyleColorsDark(Application.ig_style);

    Application.ig_base_style = *Application.ig_style;

    Application.performance_frequency = SDL_GetPerformanceFrequency();


    JSON json;

    try {
      json = JSON::from_file(config_path);
    } catch (Exception& exception) {
      exception.handle();
      json = { JSONType::Object, "Empty default config" };
    }

    
    JSONItem* fonts_item = json.get_object_item("fonts");

    if (fonts_item != NULL) {
      for (size_t i = 0; i < fonts_item->get_array().count; i ++) {
        JSONItem* font_item = fonts_item->get_array_item(i);

        String& font_path = font_item->get_object_item("path")->get_string();

        f64_t& font_size = font_item->get_object_item("size")->get_number();

        ImFont* font = Application.ig_io->Fonts->AddFontFromFileTTF(font_path.value, font_size * max_ui_scale);

        m_asset_assert(font != NULL, font_path.value, "Failed to load Font from source");

        Application.fonts.append(font);
      }
    }

    if (Application.fonts.count == 0) {
      DEFAULT_FONT_CONFIG.SizePixels = default_font_0_scale * max_ui_scale;
      Application.fonts.append(Application.ig_io->Fonts->AddFontDefault(&DEFAULT_FONT_CONFIG));
    }

    if (Application.fonts.count == 1) {
      f32_t orig = DEFAULT_FONT_CONFIG.SizePixels;
      DEFAULT_FONT_CONFIG.SizePixels = default_font_1_scale * max_ui_scale;
      Application.fonts.append(Application.ig_io->Fonts->AddFontDefault(&DEFAULT_FONT_CONFIG));
      DEFAULT_FONT_CONFIG.SizePixels = orig;
    }


    JSONItem* ui_scale_item = json.get_object_item("ui_scale");
    
    Application.set_ui_scale(ui_scale_item != NULL? ui_scale_item->get_number() : 1.0);

    
    // TODO add string value option for vsync config
    JSONItem* vsync_item = json.get_object_item("vsync");

    Application.vsync = vsync_item != NULL? vsync_item->get_number() : ApplicationVSyncMode::VBlank;

    if (Application.vsync == ApplicationVSyncMode::VBlank) {
      m_assert(SDL_GL_SetSwapInterval(1) == 0, "Failed to enable VBlank VSync mode");
    } else {
      m_assert(SDL_GL_SetSwapInterval(0) == 0, "Failed to disable VBlank VSync mode");
    }

    
    // Defaults to prevent accessing uninitialized data
    Application.window_mode = 0; 
    Application.resolution = min_resolution;


    JSONItem* display_item = json.get_object_item("display");

    Application.set_display(display_item != NULL? display_item->get_number() : 0);


    // TODO add string value option for window mode config
    JSONItem* window_mode_item = json.get_object_item("window_mode");

    Application.set_window_mode(window_mode_item != NULL? window_mode_item->get_number() : ApplicationWindowMode::Windowed);


    JSONItem* resolution_item = json.get_object_item("resolution");

    if (resolution_item != NULL) {
      Application.set_resolution({
        (s32_t) resolution_item->get_array_item(0)->get_number(),
        (s32_t) resolution_item->get_array_item(1)->get_number(),
      });
    }


    if (Application.window_mode == ApplicationWindowMode::Windowed) {
      JSONItem* position_item = json.get_object_item("position");

      if (position_item != NULL) {
        SDL_SetWindowPosition(
          Application.window,
          (s32_t) position_item->get_array_item(0)->get_number(),
          (s32_t) position_item->get_array_item(1)->get_number()
        );
      }
    }


    JSONItem* target_framerate_item = json.get_object_item("target_framerate");

    Application.target_framerate = target_framerate_item != NULL? target_framerate_item->get_number() : Application.get_native_framerate();


    JSONItem* show_fps_item = json.get_object_item("show_fps");

    Application.show_fps = show_fps_item != NULL? show_fps_item->get_boolean() : false;

    
    JSONItem* show_info_item = json.get_object_item("show_info");

    Application.show_info = show_info_item != NULL? show_info_item->get_boolean() : false;


    json.destroy();


    SDL_ShowWindow(Application.window);

    
    ImGui_ImplSDL2_InitForOpenGL(Application.window, Application.gl_context);

    // TODO use later GLSL version for ImGui?
    ImGui_ImplOpenGL3_Init("#version 130");




    return Application;
  }


  void Application_t::destroy () {
    if (config_path != NULL) {
      JSON json;

      try {
        json = JSON::from_file(config_path);
      } catch (Exception& exception) {
        exception.handle();
        json = { JSONType::Object, "New outbound application config" };
      }

      json.set_object_value(Application.ui_scale, "ui_scale");
      json.set_object_value((f64_t) Application.vsync, "vsync");
      json.set_object_value((f64_t) Application.window_mode, "window_mode");
      json.set_object_value((f64_t) SDL_GetWindowDisplayIndex(Application.window), "display");
      
      if (Application.vsync > ApplicationVSyncMode::VBlank) {
        json.set_object_value((f64_t) Application.target_framerate, "target_framerate");
      } else if (json.get_object_item("target_framerate") != NULL) {
        json.remove_object_item("target_framerate");
      }

      json.set_object_item(JSONArray::from_elements(
        JSONItem { (f64_t) Application.resolution.x },
        JSONItem { (f64_t) Application.resolution.y }
      ), "resolution");

      if (Application.window_mode == ApplicationWindowMode::Windowed) {
        Vector2s pos;

        SDL_GetWindowPosition(Application.window, &pos.x, &pos.y);

        json.set_object_item(JSONArray::from_elements(
          JSONItem { (f64_t) pos.x },
          JSONItem { (f64_t) pos.y }
        ), "position");
      } else if (json.get_object_item("position") != NULL) {
        json.remove_object_item("position");
      }

      json.set_object_value(Application.show_fps, "show_fps");
      json.set_object_value(Application.show_info, "show_info");

      if (!json.to_file(config_path)) {
        printf("Warning: Failed to save Application config file at path '%s', make sure the containing folder exists\n", config_path);
      }

      json.destroy();
    }


    ImGui_ImplOpenGL3_Shutdown();

    ImGui_ImplSDL2_Shutdown();

    ImGui::DestroyContext(Application.ig_context);


    SDL_GL_DeleteContext(Application.gl_context);

    SDL_DestroyWindow(Application.window);

    SDL_Quit();
  }



  ResolutionSet Application_t::get_resolution_set () const {
    ResolutionSet set = { { }, 0, ResolutionSet::null_index };

    s32_t display_index  = SDL_GetWindowDisplayIndex(window);

    s32_t num_display_modes = SDL_GetNumDisplayModes(display_index);

    SDL_DisplayMode active_mode;
    SDL_GetWindowDisplayMode(window, &active_mode);

    for (s32_t i = 0; i < num_display_modes; i ++) {
      SDL_DisplayMode mode;
      SDL_GetDisplayMode(display_index, i, &mode);

      if (mode.w >= min_resolution.x && mode.h >= min_resolution.y) {
        bool exists = false;

        for (ResolutionSet::Index j = 0; j < set.count; j ++) {
          Vector2s& existing_res = set[j];

          if (existing_res.x == mode.w && existing_res.y == mode.h) {
            exists = true;
            break;
          }
        }

        if (!exists) {
          set[set.count] = { mode.w, mode.h };
          
          if (set.active_index == ResolutionSet::null_index
          &&  active_mode.w == mode.w && active_mode.h == mode.h) {
            set.active_index = i;
          }

          ++ set.count;

          if (set.count == ResolutionSet::max_resolution_count) break;
        }
      }
    }

    return set;
  }

  u8_t Application_t::get_native_framerate () const {
    SDL_DisplayMode display_mode;

    SDL_GetWindowDisplayMode(window, &display_mode);

    // TODO make fallback refresh rate configurable?
    return display_mode.refresh_rate? display_mode.refresh_rate : 60;
  }


  void Application_t::begin_frame () {
    frame_start = SDL_GetPerformanceCounter();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();
  }

  void Application_t::end_frame () {
    using namespace ImGui;

    // TODO overlay flags configuration
    static constexpr
    s32_t overlay_flags = ImGuiWindowFlags_AlwaysAutoResize
                        | ImGuiWindowFlags_NoTitleBar
                        | ImGuiWindowFlags_NoInputs
                        | ImGuiWindowFlags_NoBackground;

    if (show_fps) {
      SetNextWindowPos({ ig_io->DisplaySize.x - 10, 10 }, ImGuiCond_Always, { 1, 0 });
      Begin("FPS", &show_fps, overlay_flags);;
      PushFont(fonts[1]);
      Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ig_io->Framerate, ig_io->Framerate);
      PopFont();
      End();
    }

    if (show_info) {
      SetNextWindowPos({ 10, 10 }, ImGuiCond_Always, { 0, 0 });
      Begin("Application Info", &show_info, overlay_flags);
      PushFont(fonts[1]);
      Text("Resolution: %d x %d", resolution.x, resolution.y);
      Text("Window mode: %d", window_mode);
      Text("Display index: %d", SDL_GetWindowDisplayIndex(window));
      Text("UI Scale: %g", ui_scale);
      Text("Vsync mode: %d", vsync);
      if (vsync > 1) Text("Target FPS: %d", target_framerate);
      PopFont();
      End();
    }


    SDL_GL_MakeCurrent(window, gl_context);

    glViewport(0, 0, (int)ig_io->DisplaySize.x, (int)ig_io->DisplaySize.y);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    Render();
    ImGui_ImplOpenGL3_RenderDrawData(GetDrawData());

    SDL_GL_SwapWindow(window);


    if (vsync > ApplicationVSyncMode::VBlank) {
      f64_t frame_delay = 1000.0 / (f64_t) target_framerate;

      // TODO cleanup vsync lock code
      if (vsync == ApplicationVSyncMode::SpinLock) {
        f64_t frame_time_ms;
        do {
          u64_t frame_end = SDL_GetPerformanceCounter();
          u64_t frame_time = (frame_end - frame_start) * 1000;
          frame_time_ms = (f64_t) frame_time / (f64_t) performance_frequency;
        } while (frame_delay > frame_time_ms);
      } else if (vsync == ApplicationVSyncMode::SleepLock) {
        u64_t frame_end = SDL_GetPerformanceCounter();
        u64_t frame_time = (frame_end - frame_start) * 1000;
        f64_t frame_time_ms = (f64_t) frame_time / (f64_t) performance_frequency;

        if (frame_delay > frame_time_ms) {
          SDL_Delay(frame_delay - frame_time_ms);
        }
      }
    }
  }
  

  void Application_t::set_vsync (u8_t new_vsync) {
    if (!ApplicationVSyncMode::validate(new_vsync)) {
      printf("Warning: Invalid VSync %u mode passed to Application.set_vsync; using ApplicationVSyncMode::VBlank instead\n", new_vsync);
      new_vsync = ApplicationVSyncMode::VBlank;
    }

    if (new_vsync != vsync) {
      vsync = new_vsync;

      if (vsync == ApplicationVSyncMode::VBlank) {
        m_assert(SDL_GL_SetSwapInterval(1) == 0, "Failed to enable VBlank vsync mode");
      } else {
        m_assert(SDL_GL_SetSwapInterval(0) == 0, "Failed to disable VBlank vsync mode");
      }
    }
  }

  void Application_t::set_resolution (Vector2s const& new_resolution) {
    if (window_mode == ApplicationWindowMode::FullscreenDesktop) {
      ResolutionSet res_set = get_resolution_set();
      resolution = res_set[0];
    } else if (new_resolution.x != resolution.x || new_resolution.y != resolution.y) {
      SDL_SetWindowSize(window, new_resolution.x, new_resolution.y);

      resolution = new_resolution;

      if (window_mode == ApplicationWindowMode::Windowed) {
        s32_t display_index = SDL_GetWindowDisplayIndex(window);

        SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(display_index), SDL_WINDOWPOS_CENTERED_DISPLAY(display_index));
      }
    }
  }

  void Application_t::set_display (u8_t new_display_index) {
    s32_t total_displays = SDL_GetNumVideoDisplays();

    if (new_display_index > total_displays - 1) {
      printf("Warning: Invalid display index %u passed to Application.set_display; using display 0 instead\n", new_display_index);
      new_display_index = 0;
    }

    if (new_display_index != SDL_GetWindowDisplayIndex(window)) {
      if (window_mode > ApplicationWindowMode::Windowed) SDL_SetWindowFullscreen(window, 0);
      SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED_DISPLAY(new_display_index), SDL_WINDOWPOS_CENTERED_DISPLAY(new_display_index));
      if (window_mode > ApplicationWindowMode::Windowed) SDL_SetWindowFullscreen(window, ApplicationWindowMode::to_sdl(window_mode));
    }
  }

  void Application_t::set_window_mode (u8_t new_window_mode) {
    if (!ApplicationWindowMode::validate(new_window_mode)) {
      printf("Warning: Invalid window mode %u passed to Application.set_window_mode; using ApplicationWindowMode::Windowed instead\n", new_window_mode);
      new_window_mode = ApplicationWindowMode::Windowed;
    }

    if (window_mode != new_window_mode) {
      window_mode = new_window_mode;
      SDL_SetWindowFullscreen(window, ApplicationWindowMode::to_sdl(window_mode));
      
      ResolutionSet res_set = get_resolution_set();

      if (window_mode > ApplicationWindowMode::Windowed) set_resolution(res_set[0]);
      else set_resolution(res_set[res_set.active_index]);
    }
  }

  void Application_t::set_ui_scale (f32_t new_ui_scale) {
    ui_scale = new_ui_scale;

    ig_style->WindowPadding = {
      ig_base_style.WindowPadding.x * ui_scale,
      ig_base_style.WindowPadding.y * ui_scale
    };

    ig_style->WindowMinSize = {
      ig_base_style.WindowMinSize.x * ui_scale,
      ig_base_style.WindowMinSize.y * ui_scale
    };

    ig_style->FramePadding = {
      ig_base_style.FramePadding.x * ui_scale,
      ig_base_style.FramePadding.y * ui_scale
    };

    ig_style->ItemSpacing = {
      ig_base_style.ItemSpacing.x * ui_scale,
      ig_base_style.ItemSpacing.y * ui_scale
    };

    ig_style->ItemInnerSpacing = {
      ig_base_style.ItemInnerSpacing.x * ui_scale,
      ig_base_style.ItemInnerSpacing.y * ui_scale
    };

    ig_style->TouchExtraPadding = {
      ig_base_style.TouchExtraPadding.x * ui_scale,
      ig_base_style.TouchExtraPadding.y * ui_scale
    };

    ig_style->IndentSpacing = ig_base_style.IndentSpacing * ui_scale;
    ig_style->ColumnsMinSpacing = ig_base_style.ColumnsMinSpacing * ui_scale;
    ig_style->ScrollbarSize = ig_base_style.ScrollbarSize * ui_scale;
    ig_style->GrabMinSize = ig_base_style.GrabMinSize * ui_scale;

    ig_style->DisplayWindowPadding = {
      ig_base_style.DisplayWindowPadding.x * ui_scale,
      ig_base_style.DisplayWindowPadding.y * ui_scale
    };

    ig_style->DisplaySafeAreaPadding = {
      ig_base_style.DisplaySafeAreaPadding.x * ui_scale,
      ig_base_style.DisplaySafeAreaPadding.y * ui_scale
    };

    ig_io->FontGlobalScale = ui_scale / max_ui_scale;
  }
}