#include "../../main.hh"
#include "../../../extern/imgui/implementation/main.cpp"
#include "../../../extern/ImGuiColorTextEdit/main.cpp"



static void vendor_ex (bool ig_demo = false) {
  static bool init = false;

  if (!init) {
    text_editor_demo_init();
    init = true;
  }
  
  ImGui::PushFont(mod::Application.fonts[1]);
  if (ig_demo) ig_demo_loop_body();
  text_editor_demo_loop();
  ImGui::PopFont();
}