#include "../../main.hh"


static void settings_menu_ex (u8_t prev_mode, u8_t& new_mode) {
  using namespace mod;
  using namespace ImGui;

  static constexpr ImGuiWindowFlags settings_flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar;

  f32_t win_w = Application.fonts[0]->FallbackAdvanceX * 32 * Application.ui_scale;
  f32_t win_qw = win_w * .25f;
  f32_t i_win_qw = -win_qw * Application.ui_scale;

  Vector2f win_min = { win_w * Application.ui_scale, 100 };
  Vector2f win_max = { Application.ig_io->DisplaySize.x * .75f, Application.ig_io->DisplaySize.y * .75f };
  
  SetNextWindowSizeConstraints(win_min.min(win_max), win_max.max(win_min));
  SetNextWindowPos(Vector2f(Application.ig_io->DisplaySize) / 2.0f, ImGuiCond_Always, { .5f, .5f });
  Begin("Settings", NULL, settings_flags); {
    if (CollapsingHeader("Display")) {
      /* Resolution Selection */ {
        ResolutionSet resolution_set = Application.get_resolution_set();
        ResolutionSet::Index active_index = resolution_set.active_index;

        char description [16 * ResolutionSet::max_resolution_count];
        for (ResolutionSet::Index i = 0; i < resolution_set.count; i ++) {
          snprintf(description + i * 16, 16, "%dx%d", resolution_set[i].x, resolution_set[i].y);
        }

        PushItemWidth(i_win_qw);
        if (BeginCombo("Resolution", description + active_index * 16)) {
          for (ResolutionSet::Index i = 0; i < resolution_set.count; i ++) {
            bool is_selected = i == active_index;

            if (Selectable(description + i * 16, is_selected)) {
              Application.set_resolution(resolution_set[i]);
            }

            if (is_selected) SetItemDefaultFocus();
          }
          
          EndCombo();
        }
      }


      /* Window mode */ {
        PushItemWidth(i_win_qw);
        if (BeginCombo("Window Mode", ApplicationWindowMode::name(Application.window_mode))) {
          for (u8_t mode = 0; mode < ApplicationWindowMode::total_mode_count; mode ++) {
            bool is_selected = mode == Application.window_mode;

            if (Selectable(ApplicationWindowMode::name(mode), is_selected)) {
              Application.set_window_mode(mode);
            }

            if (is_selected) SetItemDefaultFocus();
          }

          EndCombo();
        }
      }


      /* Display selection */ {
        s32_t num_displays = num::min(Application.get_display_count(), 10);
        s32_t display_index = Application.get_display_index();
        char name [16 * 10];

        for (s32_t i = 0; i < num_displays; i ++) {
          itoa(i, name + 16 * i, 10);
        }
        
        PushItemWidth(i_win_qw);
        if (BeginCombo("Display", display_index < num_displays? name + display_index * 16 : "Out of range")) {
          for (s32_t i = 0; i < num_displays; i ++) {
            bool is_selected = i == display_index;

            if (Selectable(name + i * 16, is_selected)) {
              Application.set_display(i);
            }

            if (is_selected) SetItemDefaultFocus();
          }

          EndCombo();
        }
      }

      
      /* FPS Control */ {
        u8_t vsync_mode = Application.vsync;

        static constexpr char const* help_texts [4] = {
          "None - Does not attempt to limit the framerate",
          "VBlank - Uses a standard method of VSync included with the Simple DirectMedia Layer library, this is recommended but has shown to lock to incorrect rates on some hardware",
          "SleepLock - Uses a processor sleep call to lock FPS, only works in whole milliseconds so FPS will not always match the target, but uses less energy than spin lock",
          "SpinLock - Uses a loop to lock FPS, this is more accurate than sleep lock but does not allow the processor to idle, requiring more power"
        };

        PushItemWidth(i_win_qw);
        if (BeginCombo("FPS Lock", ApplicationVSyncMode::name(vsync_mode))) {
          for (u8_t mode = 0; mode < ApplicationVSyncMode::total_mode_count; mode ++) {
            bool is_selected = mode == vsync_mode;

            if (Selectable(ApplicationVSyncMode::name(mode), is_selected)) {
              Application.set_vsync(mode);
            }

            HelpMarker(help_texts[mode]);

            if (is_selected) SetItemDefaultFocus();
          }

          EndCombo();
        }

        static constexpr u8_t min_fps = 24;
        static constexpr u8_t max_fps = 240;
        if (vsync_mode == ApplicationVSyncMode::SleepLock || vsync_mode == ApplicationVSyncMode::SpinLock) {
          SliderScalar("Target FPS", ImGuiDataType_U8, &Application.target_framerate, &min_fps, &max_fps, "%" PRIu8);
        } else {
          u8_t target_framerate = vsync_mode == ApplicationVSyncMode::None? 0 : Application.target_framerate;
          PushStyleVar(ImGuiStyleVar_Alpha, Application.ig_style->Alpha * .5f);
          SliderScalar("Target FPS", ImGuiDataType_U8, &target_framerate, &min_fps, &max_fps, "%" PRIu8 " (Disabled)");
          PopStyleVar();
        }
      }


      /* UI Scale */ {
        static constexpr f32_t scales [7] = {
          0.5,
          0.75,
          1.0,
          1.25,
          1.5,
          1.75,
          2.0
        };

        static constexpr char const* names [7] = {
          ".5x",
          ".75x",
          "1x",
          "1.25x",
          "1.5x",
          "1.75x",
          "2x"
        };

        char orig_name [16];
        snprintf(orig_name, 16, "%gx", Application.ui_scale);

        PushItemWidth(i_win_qw);
        if (BeginCombo("UI Scale", orig_name)) {
          for (u8_t i = 0; i < 7; i ++) {
            bool is_selected = num::flt_equal(scales[i], Application.ui_scale);

            if (Selectable(names[i], is_selected)) {
              Application.set_ui_scale(scales[i]);
            }

            if (is_selected) SetItemDefaultFocus();
          }

          EndCombo();
        }
      }
    }

    if (CollapsingHeader("Debug")) {
      Checkbox("Display FPS", &Application.show_fps);
      Checkbox("Display App Info", &Application.show_info);
    }

    if (CollapsingHeader("Controls")) {
      Input.show_binding_menu();
      Input.show_binding_modal();
    }

    f32_t w = GetContentRegionAvailWidth() / 2.0f;
    NewLine();
    SameLine(w, Application.ig_style->ItemSpacing.x);
    if (Button("Back", { w, 0 })) new_mode = prev_mode;
  } End();
}