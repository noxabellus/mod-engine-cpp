#include "main.hh"

#define LIB_PATH ".\\mods\\mod"

Version game_version = { 0, 0, 1 };

s32_t main (s32_t, char**) {
  using namespace mod;

  #ifdef MEMORY_DEBUG_INDEPTH
    StringStackWalker.init();
  #endif

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

    SharedLib lib { LIB_PATH };

    auto module_init = reinterpret_cast<void (*) ()>(lib.get_address("module_init"));

    m_asset_assert(module_init != NULL, lib.origin, "Failed to get module_init function");

    module_init();

    lib.destroy();
  } catch (Exception& except) {
    except.panic();
  }

  if (memory::allocated_size != 0 || memory::allocation_count != 0) {
    #ifdef MEMORY_DEBUG_INDEPTH
      #define MEM_DUMP_PATH ".\\mem_dump.txt"
      printf("Warning: Possible memory leak detected dumping allocation data to file %s\n", MEM_DUMP_PATH);
      FILE* f = fopen(MEM_DUMP_PATH, "w");
      memory::dump_allocation_data(f);
      fclose(f);
    #else
      printf("Warning: Possible memory leak detected\n");
      memory::dump_allocation_data();
    #endif
  }
  
  return 0;
}