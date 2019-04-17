#include "..\main.hh"
#include "..\..\source\extern\imgui\implementation\main.cpp"
#include "..\..\source\extern\ImGuiColorTextEdit\main.cpp"

using namespace mod;



MODULE_API void module_init () {
  Application.create();
  
  text_editor_demo_init();

  while (true) {
    SDL_Event event;
    bool quit = false;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT) {
        quit = true;
        break;
      }
    }
    if (quit) break;

    Application.begin_frame();

    ImGui::PushFont(Application.fonts[1]);
    ig_demo_loop_body();
    text_editor_demo_loop();
    ImGui::PopFont();

    Application.end_frame();
  }

  Application.destroy();
}