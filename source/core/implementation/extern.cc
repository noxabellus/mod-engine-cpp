#include "../include/ModEngine.hh"

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

#include <extern/tinycthread/tinycthread.c>
#include <extern/gl3w/gl3w.c>
#include <extern/imgui/implementation/imgui_impl_sdl.cpp>
#include <extern/imgui/implementation/imgui_impl_opengl3.cpp>

#pragma clang diagnostic pop


#ifdef MEMORY_DEBUG_INDEPTH
  using StackTraceMsg = Name<512>;

  ENGINE_API extern mod::Array<StackTraceMsg> stack_trace_msgs;
  mod::Array<StackTraceMsg> stack_trace_msgs = mod::Array<StackTraceMsg> { 0, true };

  ENGINE_API extern bool ssw_init;
  bool ssw_init = false;

  StringStackWalker_t StringStackWalker;

  StringStackWalker_t& StringStackWalker_t::init () {
    StringStackWalker.~StringStackWalker_t();
    new (&StringStackWalker) StringStackWalker_t { };
    ssw_init = true;
    return StringStackWalker;
  }

  bool StringStackWalker_t::ShowCallstack () {
    if (!ssw_init) return false;
    else return StackWalker::ShowCallstack();
  }

  size_t StringStackWalker_t::max_msgs = 16;

  void StringStackWalker_t::OnOutput (LPCSTR text) { 
    if (!ssw_init) return;
    if (stack_trace_msgs.count >= max_msgs) stack_trace_msgs.remove(0);
    stack_trace_msgs.append(text);
  }
    
  ENGINE_API extern mod::String stack_trace;
  mod::String stack_trace = mod::String { 128, true };


  char const* StringStackWalker_t::CreateStackStr () {
    if (!ssw_init) return "StackWalker uninitialized, value is possibly static";

    stack_trace.clear();

    for (auto [ i, msg ] : stack_trace_msgs) {
      char const* text = msg.value;

      if (text[1] == ':') {
        bool path = false;
        s32_t path_end = 0;
        bool addr_line = false;
        s32_t addr_line_end = 0;
        s32_t name_end = 0;
        bool started_name = false;
        for (s32_t i = 0, max = strlen(text); i < max; i ++) {
          if (!path) {
            if (text[i] == '(') {
              path = true;
              path_end = i;
            } 
          } else if (!addr_line) {
            if (text[i] == ')') {
              addr_line = true;
              addr_line_end = i;
            }
          } else if (!started_name) {
            if (text[i] == ':') {
              started_name = true;
              i += 1; // skip space
            } else {
              break;
            }
          } else {
            if (mod::char_is_whitespace(text[i])) {
              name_end = i;
              break;
            }
          }
        }

        stack_trace.fmt_append(
          "%.*s",
          path_end - 1,   text
        );

        bool source_is_text = stack_trace.ends_with(".cpp")
                          || stack_trace.ends_with(".cc")
                          || stack_trace.ends_with(".c")
                          || stack_trace.ends_with(".hh")
                          || stack_trace.ends_with(".hpp")
                          || stack_trace.ends_with(".h")
                          || stack_trace.ends_with(".inl");

        stack_trace.fmt_append(
          ":%.*s",
          addr_line_end - path_end - 1,   text + path_end + 1
        );

        if (source_is_text) stack_trace.fmt_append(
          " %.*s\n",
          name_end - addr_line_end - 3,   text + addr_line_end + 3
        ); else stack_trace.append("\n");
      }
    }
    
    stack_trace_msgs.clear();
    
    return stack_trace.value;
  }
#endif