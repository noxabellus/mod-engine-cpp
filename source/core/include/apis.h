#ifndef APIS_H
#define APIS_H

#ifdef _WIN32
  #if defined(M_ENGINE)
    #if defined(M_GAME) || defined(M_MODULE)
      #error Only one of M_ENGINE, M_GAME, or M_MODULE may be defined while compiling
    #endif
    #define ENGINE_API __declspec(dllexport)
    #define GAME_API __declspec(dllimport)
    #define MODULE_API extern "C" __declspec(dllimport)
  #elif defined(M_GAME)
    #if defined(M_ENGINE) || defined(M_MODULE)
      #error Only one of M_ENGINE, M_GAME, or M_MODULE may be defined while compiling
    #endif
    #define ENGINE_API __declspec(dllimport)
    #define GAME_API __declspec(dllexport)
    #define MODULE_API extern "C" __declspec(dllimport)
  #elif defined(M_MODULE)
    #if defined(M_ENGINE) || defined(M_GAME)
      #error Only one of M_ENGINE, M_GAME, or M_MODULE may be defined while compiling
    #endif
    #define ENGINE_API __declspec(dllimport)
    #define GAME_API __declspec(dllimport)
    #define MODULE_API extern "C" __declspec(dllexport)
  #elif defined(__INTELLISENSE__)
    #define ENGINE_API
    #define GAME_API
    #define MODULE_API extern "C"
  #else
    #error One of either M_ENGINE, M_GAME, or M_MODULE must be defined while compiling
  #endif
#else
  #define ENGINE_API
  #define GAME_API
  #define MODULE_API extern "C"
#endif

#endif