#include "../include/SharedLib.hh"


#ifdef _WIN32
  #include "Windows.h"

  namespace mod {
    SharedLib::SharedLib (char const* path, size_t length) {
      origin = str_clone(path, length);

      platform_ptr = LoadLibraryA(origin);

      m_asset_assert((size_t) platform_ptr != -1_u64 && (size_t) platform_ptr != SIZE_MAX, origin, "Failed to load SharedLib, Windows error code was %" PRIu32, (u32_t) GetLastError());
    }

    void SharedLib::destroy () {
      m_asset_assert(FreeLibrary((HMODULE) platform_ptr) != 0, origin, "Failed to unload SharedLib, Windows error code was %" PRIu32, (u32_t) GetLastError());
      free(origin);
    }


    void* SharedLib::get_address (char const* name) {
      return (void*) GetProcAddress((HMODULE) platform_ptr, name);
    }
  }
#else
  #include "dlfcn.h"

  SharedLib::SharedLib (char const* path, size_t length) {
    origin = str_clone(path, length);
    platform_ptr = dlopen(origin, RTLD_LAZY);

    m_asset_assert(platform_ptr != NULL, origin, "Failed to load SharedLib, POSIX error message was '%s'", dlerror());
  }

  void SharedLib::destroy () {
    m_asset_assert(dlclose(platform_ptr) == 0, origin, "Failed to unload SharedLib, POSIX error message was '%s'", dlerror());
    free(origin);
  }


  void* SharedLib::get_address (char const* name) {
    return dlsym(platform_ptr, name);
  }
#endif