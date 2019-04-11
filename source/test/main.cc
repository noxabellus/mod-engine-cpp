#include "main.hh"

Version game_version = { 0, 0, 1 };

#define LIB_PATH ".\\mods\\mod"

s32_t main (s32_t argc, char** args) {
  try {
    printf(
      "ModEngine\n"
      "Test Game"
      #ifdef DEBUG
        " Debug "
      #elif RELEASE
        " Release "
      #else
        "\n\nUnknown Target!\n\n"
      #endif
      "Version %u.%u.%u\n",
      game_version.major, game_version.minor, game_version.patch
    );

    mod::SharedLib lib { LIB_PATH };

    auto module_init = (void (*) ()) lib.get_address("module_init");

    m_asset_assert(module_init != NULL, lib.origin, "Failed to get module_init function");

    module_init();

    lib.destroy();
  } catch (mod::Exception& except) {
    except.panic();
  }
  
  return 0;
}