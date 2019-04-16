#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <climits>
#include <cinttypes>
#include <cstdbool>
#include <cstring>
#include <cfloat>
#include <cmath>
#include <ctime>



#include <new>
#include <typeinfo>
#include <type_traits>
#include <limits>


#include "extern.hh"


namespace mod { }

using f128_t = long double;
using f64_t = double;
using f32_t = float;

using  u8_t = uint8_t;
using u16_t = uint16_t;
using u32_t = uint32_t;
using u64_t = uint64_t;

using  s8_t = int8_t;
using s16_t = int16_t;
using s32_t = int32_t;
using s64_t = int64_t;


/* Allows for literals like `-1_s64` to be interpreted as a single unit */
template <typename T> struct SignWrapper {
  T v = 0;

  inline constexpr
  operator T () { return v; }

  inline constexpr
  SignWrapper operator - () { return { (T) -v }; }
};


/* Allows for literals like `-1_s8` to be interpreted as a single unit */
using s8_w  = SignWrapper<s8_t>;
/* Allows for literals like `-1_s16` to be interpreted as a single unit */
using s16_w = SignWrapper<s16_t>;
/* Allows for literals like `-1_s32` to be interpreted as a single unit */
using s32_w = SignWrapper<s32_t>;
/* Allows for literals like `-1_s64` to be interpreted as a single unit */
using s64_w = SignWrapper<s64_t>;


/* Type suffix for consistent, explicit literals */
inline constexpr s8_w  operator ""_s8  (u64_t i) { return { (s8_t)  i }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s16_w operator ""_s16 (u64_t i) { return { (s16_t) i }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s32_w operator ""_s32 (u64_t i) { return { (s32_t) i }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s64_w operator ""_s64 (u64_t i) { return { (s64_t) i }; }


/* Type suffix for consistent, explicit literals */
inline constexpr u8_t  operator ""_u8  (u64_t i) { return (u8_t)  i; }
/* Type suffix for consistent, explicit literals */
inline constexpr u16_t operator ""_u16 (u64_t i) { return (u16_t) i; }
/* Type suffix for consistent, explicit literals */
inline constexpr u32_t operator ""_u32 (u64_t i) { return (u32_t) i; }
/* Type suffix for consistent, explicit literals */
inline constexpr u64_t operator ""_u64 (u64_t i) { return (u64_t) i; }


template <typename A, typename B = A>
struct pair_t {
  A a;
  B b;
  
  template <typename U, typename V> operator pair_t<U, V> () { return { a, b }; }
  bool operator == (pair_t const& other) { return a == other.a && b == other.b; }
  bool operator != (pair_t const& other) { return a != other.a || b != other.b; }
};

template <typename A, typename B = A, typename C = A>
struct tri_t {
  A a;
  B b;
  C c;

  template <typename U, typename V, typename W> operator tri_t<U, V, W> () { return { a, b, c }; }
  bool operator == (tri_t const& other) { return a == other.a && b == other.b && c == other.c; }
  bool operator != (tri_t const& other) { return a != other.a || b != other.b || c != other.c; }
};

template <typename A, typename B = A, typename C = A, typename D = A>
struct quad_t {
  A a;
  B b;
  C c;
  D d;

  template <typename U, typename V, typename W, typename X> operator quad_t<U, V, W, X> () { return { a, b, c, d }; }
  bool operator == (quad_t const& other) { return a == other.a && b == other.b && c == other.c && d == other.d; }
  bool operator != (quad_t const& other) { return a != other.a || b != other.b || c != other.c || d != other.d; }
};

struct Version {
  u8_t major;
  u8_t minor;
  u8_t patch;
};


/* Runtime panic with printf formatted error message */
#define m_error(FMT, ...) { \
  fprintf(stderr, "Internal error at [%s:%d]: ", __FILE__, __LINE__); \
  fprintf(stderr, FMT, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  abort(); \
}

/* Runtime assertion with panic and printf formatted error message if the boolean condition is not true */
#define m_assert(COND, FMT, ...) if (!(COND)) m_error(FMT, __VA_ARGS__)


namespace mod {
  struct Exception {
    char* file = NULL;
    s32_t line = -1;
    s32_t column = -1;

    char* message = NULL;

    void handle () {
      if (file != NULL) free(file);
      if (message != NULL) free(message);
    }

    void print (FILE* stream = stderr) {
      fprintf(stream, "Exception at [");

      if (file != NULL) {
        fprintf(stream, "%s", file);
      }

      if (line != -1) {
        if (file != NULL) fprintf(stream, ":");

        fprintf(stream, "%" PRId32, line);

        if (column != -1) {
          fprintf(stream, ":");
          fprintf(stream, "%" PRId32, column);
        }
      }

      fprintf(stream, "]");

      if (message != NULL) {
        fprintf(stream, ": %s", message);
      }

      fprintf(stream, "\n");
    }

    void panic (FILE* stream = stderr) {
      fprintf(stream, "Unhandled ");
      print(stream);
      handle();
      abort();
    }
  };


  /* Wrapper for snprintf that produces a new heap-allocated str */
  static char* str_fmt_va (char const* fmt, va_list args) {
    va_list args_copy;

    va_copy(args_copy, args);

    int length = vsnprintf(NULL, 0, fmt, args_copy) + 1;

    va_end(args_copy);

    auto out = (char*) calloc(1, length);

    vsnprintf(out, length, fmt, args);

    return out;
  }

  /* Wrapper for vsnprintf that produces a new heap-allocated str */
  static char* str_fmt (char const* fmt, ...) {
    va_list args;
    
    va_start(args, fmt);

    char* result = str_fmt_va(fmt, args);

    va_end(args);
    
    return result;
  }

  /* Create a new heap-allocated str copy of a str or substr */
  static char* str_clone (char const* base, size_t length = 0) {
    if (base == NULL) return NULL;
    
    if (length == 0) length = strlen(base);
    
    auto out = (char*) malloc(length + 1);

    memcpy(out, base, length);

    out[length] = '\0';

    return out;
  }
}


/* Determine the length of an array */
#define m_array_length(ARR) (sizeof(ARR) / sizeof(ARR[0]))

/* Get the address offset of a structure's field */
#define m_field_offset(TY, FIELD) ((size_t) (&((TY*) 0)->FIELD))


namespace num {
  /* Get the absolute value of a number */
  template <typename T> constexpr T abs (T v) {
    return v < T(0)? -v : v;
  }

  /* Get the minimum value of a set of numbers */
  template <typename T, typename ... A> constexpr T min (T arg0, A ... argsN) {
    constexpr size_t arg_count = sizeof...(argsN);
    T args [arg_count] = { ((T) argsN)... };
    for (size_t i = 0; i < arg_count; i ++) {
      if (args[i] < arg0) arg0 = args[i];
    }
    return arg0;
  }

  /* Get the maximum value of a set of numbers */
  template <typename T, typename ... A> constexpr T max (T arg0, A ... argsN) {
    constexpr size_t arg_count = sizeof...(argsN);
    T args [arg_count] = { ((T) argsN)... };
    for (size_t i = 0; i < arg_count; i ++) {
      if (args[i] > arg0) arg0 = args[i];
    }
    return arg0;
  }

  /* Get the minimum value of an array of numbers */
  template <typename T> T min_element (T const* buff, size_t element_count) {
    T m = buff[0];
    for (size_t i = 0; i < element_count; i ++) {
      if (buff[i] < m) m = buff[i];
    }
    return m;
  }

  /* Get the maximum value of an array of numbers */
  template <typename T> T max_element (T const* buff, size_t element_count) {
    T m = buff[0];
    for (size_t i = 0; i < element_count; i ++) {
      if (buff[i] > m) m = buff[i];
    }
    return m;
  }

  /* Get the sign of number */
  template <typename T> constexpr T sign (T v) {
    return v < T(0)? T(-1) : T(1);
  }

  /* Clamp a value between some minimum and maximum */
  template <typename T> constexpr T clamp (T value, T vmin, T vmax) {
    return max(vmin, min(value, vmax));
  }

  /* Linear interpolate between two numbers using an alpha (Recommend alpha be floating point 0 to 1) */
  template <typename T, typename U> constexpr T lerp (U alpha, T start, T finish) {
    T vmin = min(start, finish);
    T vmax = max(start, finish);
    return vmin + ((U) (vmax - vmin)) * alpha;
  }

  /* Get a random number between 0 and std::numeric_limts<T>::max() */
  template <typename T> T random () {
    return random<f64_t>() * std::numeric_limits<T>::max();
  }

  /* Get a random 128 bit floating point number between 0 and 1 */
  template <> f128_t random () {
    return ((f128_t) rand() / (f128_t) RAND_MAX);
  }

  /* Get a random 64 bit floating point number between 0 and 1 */
  template <> f64_t random () {
    return ((f64_t) rand() / (f64_t) RAND_MAX);
  }
  
  /* Get a random 32 bit floating point number between 0 and 1 */  
  template <> f32_t random () {
    return ((f32_t) rand() / (f32_t) RAND_MAX);
  }

  /* Get a random number between some minimum and maximum */
  template <typename T> T random_range (T vmin, T vmax) {
    return lerp<T, f64_t>(random<f64_t>(), vmin, vmax);
  }

  /* Get a random number between some minimum and maximum */
  template <> f128_t random_range (f128_t vmin, f128_t vmax) {
    return lerp<f128_t, f128_t>(random<f128_t>(), vmin, vmax);
  }

  /* Get a random number between some minimum and maximum */
  template <> f32_t random_range (f32_t vmin, f32_t vmax) {
    return lerp<f32_t, f32_t>(random<f32_t>(), vmin, vmax);
  }

  /* Seed the cmath random number generator */
  inline void seed_random (u32_t seed = time(NULL)) {
    srand(seed);
  }

  /* Get the remainder of the division of two numbers */
  template <typename T> T remainder (T l, T r) {
    if constexpr (std::is_floating_point<T>::value) {
      if constexpr (sizeof(T) <= sizeof(f32_t)) {
        return fmodf(l, r);
      } else if constexpr (sizeof(T) <= sizeof(f64_t)) {
        return fmod(l, r);
      } else {
        return fmodl(l, r);
      }
    } else {
      return l % r;
    }
  }
}


/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (char const* file, s32_t line, s32_t column, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .line = line, .column = column, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (char const* file, s32_t line, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .line = line, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (char const* file, char const* fmt, A ... args) {
  throw mod::Exception { .file = mod::str_clone(file), .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (s32_t line, s32_t column, char const* fmt, A ... args) {
  throw mod::Exception { .line = line, .column = column, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (s32_t line, char const* fmt, A ... args) {
  throw mod::Exception { .line = line, .message = mod::str_fmt(fmt, args...) };
}

/* Runtime exception with formatted error message */
template <typename ... A>
void m_asset_error (char const* fmt, A ... args) {
  throw mod::Exception { .message = mod::str_fmt(fmt, args...) };
}

/* Runtime assertion with exception using formatted error message if the boolean condition is not true */
#define m_asset_assert(COND,  ...) if (!(COND)) m_asset_error(__VA_ARGS__)