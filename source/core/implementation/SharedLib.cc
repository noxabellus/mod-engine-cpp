#include "../include/SharedLib.hh"


#ifdef _WIN32
  #include "Windows.h"

  namespace mod {
    SharedLib::SharedLib (char const* path, size_t length) {
      origin = str_clone(path, length);

      platform_ptr = LoadLibraryA(origin);

      auto platform_ptr_i = reinterpret_cast<size_t>(platform_ptr);
      m_asset_assert(platform_ptr_i != -1_u64 && platform_ptr_i != SIZE_MAX, origin, "Failed to load SharedLib, Windows error code was %lu", GetLastError());
    }

    void SharedLib::destroy () {
      m_asset_assert(FreeLibrary(reinterpret_cast<HMODULE>(platform_ptr)) != 0, origin, "Failed to unload SharedLib, Windows error code was %lu", GetLastError());
      memory::deallocate(origin);
    }


    void* SharedLib::get_address (char const* name) {
      return reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(platform_ptr), name));
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
    memory::deallocate(origin);
  }


  void* SharedLib::get_address (char const* name) {
    return dlsym(platform_ptr, name);
  }
#endif