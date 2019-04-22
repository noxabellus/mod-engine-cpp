#include "../../main.hh"
#include "settings_menu_ex.cc"


void main_menu_ex () {
  using namespace mod;
  using namespace ImGui;

  static constexpr u8_t main_menu = 0;
  static constexpr u8_t settings_menu = 1;

  static constexpr ImGuiWindowFlags main_menu_flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;

  static u8_t mode = 0;
  static u8_t new_mode = 0;
  static u8_t prev_mode = 0;

  static bool show_demo_window = false;


  if (new_mode != mode) {
    prev_mode = mode;
    mode = new_mode;
  }


  if (show_demo_window) ShowDemoWindow(&show_demo_window);


  switch (mode) {
    case main_menu: {
      f32_t h = GetFrameHeight();

      SetNextWindowSize({
        Application.fonts[0]->FallbackAdvanceX * 8 * Application.ui_scale,
        h * 8 + (Application.ig_style->WindowPadding.y * 2) + (Application.ig_style->ItemInnerSpacing.y * 7)
      }, ImGuiCond_Always);
      SetNextWindowPos({
        Application.ig_io->DisplaySize.x - 25 * Application.ui_scale,
        Application.ig_io->DisplaySize.y - 25 * Application.ui_scale
      }, ImGuiCond_Always, { 1, 1 });
      Begin("Main Menu", NULL, main_menu_flags); {
        ImVec2 fill = { GetContentRegionAvailWidth(), 0 };

        Button("Continue", fill);
        Button("New Game", fill);
        Button("Load Game", fill);
        Button("Mods", fill);
        if (Button("Settings", fill)) new_mode = settings_menu;
        if (Button("Credits", fill)) {
          #ifdef _WIN32
            system("start https://github.com/noxabellus/mod-engine/#code-basis");
          #else
            system("xdg-open https://github.com/noxabellus/mod-engine/#code-basis");
          #endif
        }
        if (Button("UI Demo", fill)) show_demo_window = !show_demo_window;
        if (Button("Exit", fill)) {
          SDL_Event quit_event;
          quit_event.type = SDL_QUIT;
          quit_event.quit.timestamp = SDL_GetTicks();
          SDL_PushEvent(&quit_event);
        }
      } End();
      
    } break;

    case settings_menu: settings_menu_ex(prev_mode, new_mode); break;
  }
}