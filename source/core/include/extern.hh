#ifndef EXTERN_H
#define EXTERN_H

#include "apis.h"

#ifndef NOMINMAX
#define NOMINMAX
#endif


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Wmissing-noreturn"
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic ignored "-Wnonportable-system-include-path"
#pragma clang diagnostic ignored "-Wlanguage-extension-token"
#pragma clang diagnostic ignored "-Wdocumentation"
#pragma clang diagnostic ignored "-Wundef"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wpragma-pack"


#include <extern/tinycthread/tinycthread.h>

#define mtx_init_safe(mtx, type) \
  m_assert(mtx_init(mtx, type) == thrd_success, "Failed to initialize Mutex")

#define mtx_lock_safe(mtx) \
  m_assert(mtx_lock(mtx) == thrd_success, "Failed to lock Mutex")

#define mtx_unlock_safe(mtx) \
  m_assert(mtx_unlock(mtx) == thrd_success, "Failed to unlock Mutex")

#define thrd_create_safe(thrd, func, arg) \
  m_assert(thrd_create(thrd, func, arg) == thrd_success, "Failed to create Thread")

#define thrd_join_safe(thrd, res) \
  m_assert(thrd_join(thrd, res) == thrd_success, "Failed to join Thread")

#define thrd_sleep_safe(duration, remaining) \
  m_assert(thrd_sleep(duration, remaining) == 0, "Failed to sleep Thread")


#include <extern/sdl2/include/SDL.h>

#undef main


#include <extern/freeimage/FreeImage.h>


#include <extern/gl3w/include/GL/gl3w.h>


#define IMGUI_DEFINE_MATH_OPERATORS
#include <extern/imgui/include/imgui.h>
#include <extern/imgui/include/imgui_impl_sdl.h>
#include <extern/imgui/include/imgui_impl_opengl3.h>

namespace ImGui {
  /* Create a ui element to offer a textual tooltip */
  static void HelpMarker (char const* help_text) {
    SameLine(0, GetStyle().ItemSpacing.x);
    TextDisabled("(?)");
    if (IsItemHovered(0)) {
      BeginTooltip();
      PushTextWrapPos(GetFontSize() * 35.0f);
      TextUnformatted(help_text, 0);
      PopTextWrapPos();
      EndTooltip();
    }
  }
}


#include <extern/ImGuiColorTextEdit/TextEditor.h>

#pragma clang diagnostic pop

#endif