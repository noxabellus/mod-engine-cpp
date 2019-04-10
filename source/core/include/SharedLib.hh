#pragma once

#include "cstd.hh"


namespace mod {
  struct SharedLib {
    char* origin = NULL;

    void* platform_ptr = NULL;

    /* Create a zero-initialized SharedLib with no loaded binary */
    SharedLib () = default;


    /* Create a SharedLib by loading a binary file.
     * Throws if the file could not be loaded or on platform-specific errors */
    ENGINE_API SharedLib (char const* path, size_t length = 0);

    /* Unload a SharedLib's binary file */
    ENGINE_API void destroy ();

    /* Get the address of a global variable or function from a SharedLib */
    ENGINE_API void* get_address (char const* name);
  };
}