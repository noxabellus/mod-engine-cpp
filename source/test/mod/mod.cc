#include "..\main.hh"
#include "..\..\source\extern\imgui\implementation\main.cpp"
#include "..\..\source\extern\ImGuiColorTextEdit\main.cpp"


MODULE_API void module_init () {
  if (ig_demo_init()) {
    text_editor_demo_init();

    while (ig_demo_loop_start()) {
      ig_demo_loop_body();
      text_editor_demo_loop();
      ig_demo_loop_end();
    }

    ig_demo_end();
  }
}