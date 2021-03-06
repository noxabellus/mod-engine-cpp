#ifndef CSTD_H
#define CSTD_H

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
#include <functional>


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
  SignWrapper operator - () { return { -v }; }
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
inline constexpr s8_w  operator ""_s8  (u64_t i) { return { static_cast<s8_t>(i) }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s16_w operator ""_s16 (u64_t i) { return { static_cast<s16_t>(i) }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s32_w operator ""_s32 (u64_t i) { return { static_cast<s32_t>(i) }; }
/* Type suffix for consistent, explicit literals */
inline constexpr s64_w operator ""_s64 (u64_t i) { return { static_cast<s64_t>(i) }; }


/* Type suffix for consistent, explicit literals */
inline constexpr u8_t  operator ""_u8  (u64_t i) { return static_cast<u8_t>(i); }
/* Type suffix for consistent, explicit literals */
inline constexpr u16_t operator ""_u16 (u64_t i) { return static_cast<u16_t>(i); }
/* Type suffix for consistent, explicit literals */
inline constexpr u32_t operator ""_u32 (u64_t i) { return static_cast<u32_t>(i); }
/* Type suffix for consistent, explicit literals */
inline constexpr u64_t operator ""_u64 (u64_t i) { return static_cast<u64_t>(i); }


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



/* printf with no warning about nonliteral format strings */
template <typename ... A> int printf_nonliteral (char const* const fmt, A ... args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return printf(fmt, args...);
  #pragma clang diagnostic pop
}

/* printf with no warning about nonliteral format strings */
template <typename ... A> int fprintf_nonliteral (FILE* const stream, char const* const fmt, A ... args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return fprintf(stream, fmt, args...);
  #pragma clang diagnostic pop
}

/* sprintf with no warning about nonliteral format strings */
template <typename ... A> int sprintf_nonliteral (char* const buff, char const* const fmt, A ... args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return sprintf(buff, fmt, args...);
  #pragma clang diagnostic pop
}

/* vsprintf with no warning about nonliteral format strings */
static inline int vsprintf_nonliteral (char* const buff, char const* const fmt, va_list args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return vsprintf(buff, fmt, args);
  #pragma clang diagnostic pop
}

/* snprintf with no warning about nonliteral format strings */
template <typename ... A> int snprintf_nonliteral (char* const buff, size_t n, char const* const fmt, A ... args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return snprintf(buff, n, fmt, args...);
  #pragma clang diagnostic pop
}

/* vsnprintf with no warning about nonliteral format strings */
static inline int vsnprintf_nonliteral (char* const buff, size_t n, char const* const fmt, va_list args) {
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wformat-nonliteral"
  return vsnprintf(buff, n, fmt, args);
  #pragma clang diagnostic pop
}


/* Runtime panic with printf formatted error message */
#define m_error(FMT, ...) { \
  fprintf(stderr, "Internal error at [%s:%d]: ", __FILE__, __LINE__); \
  fprintf(stderr, FMT, __VA_ARGS__); \
  fprintf(stderr, "\n"); \
  abort(); \
}

/* Runtime assertion with panic and printf formatted error message if the boolean condition is not true */
#define m_assert(COND, FMT, ...) if (!(COND)) m_error(FMT, __VA_ARGS__)


/* Runtime panic with printf_nonliteral formatted error message */
#define m_error_nonliteral(FMT, ...) { \
  fprintf_nonliteral(stderr, "Internal error at [%s:%d]: ", __FILE__, __LINE__); \
  fprintf_nonliteral(stderr, FMT, __VA_ARGS__); \
  fprintf_nonliteral(stderr, "\n"); \
  abort(); \
}

/* Runtime assertion with panic and printf_nonliteral formatted error message if the boolean condition is not true */
#define m_assert_nonliteral(COND, FMT, ...) if (!(COND)) m_error_nonliteral(FMT, __VA_ARGS__)


#define M_PIF static_cast<f32_t>(M_PI)

namespace num {
  /* Get the absolute value of a number */
  template <typename T> constexpr T abs (T v) {
    if constexpr (std::numeric_limits<T>::is_integer) {
      return v < T(0)? -v : v;
    } else {
      return ::abs(v);
    }
  }

  /* Determine whether two floating point values are essentially equivalent.
   * The machine epsilon has to be scaled to the magnitude of the values used,
   * and multiplied by the desired precision of units in the last place (Defaults to 2) */
  template <typename T> constexpr typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type almost_equal (T x, T y, s32_t ulp = 2) {
    return abs(x - y) <= std::numeric_limits<T>::epsilon() * abs(x + y) * ulp
        || abs(x - y) <  std::numeric_limits<T>::min();
  }

  /* Determine if two floating point values are exactly equivalent.
   * This is unsafe in most cases, so Clang has to be instructed not to warn about it.
   * This should only be used if you know its safe. The canonical case is comparing to 0 to prevent division by zero */
  template <typename T> constexpr bool flt_equal (T x, T y) {
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wfloat-equal"
    return x == y;
    #pragma clang diagnostic pop
  }

  /* Determine if two floating point values are not exactly equivalent.
   * This is unsafe in most cases, so Clang has to be instructed not to warn about it.
   * This should only be used if you know its safe. The canonical case is comparing to 0 to prevent division by zero */
  template <typename T> constexpr bool flt_not_equal (T x, T y) {
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wfloat-equal"
    return x != y;
    #pragma clang diagnostic pop
  }

  /* Get the minimum value of a set of numbers */
  template <typename T, typename ... A> constexpr T min (T arg0, A ... argsN) {
    constexpr size_t arg_count = sizeof...(argsN);
    T args [arg_count] = { static_cast<T>(argsN)... };
    for (size_t i = 0; i < arg_count; i ++) {
      if (args[i] < arg0) arg0 = args[i];
    }
    return arg0;
  }

  /* Get the maximum value of a set of numbers */
  template <typename T, typename ... A> constexpr T max (T arg0, A ... argsN) {
    constexpr size_t arg_count = sizeof...(argsN);
    T args [arg_count] = { static_cast<T>(argsN)... };
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
    return vmin + static_cast<U>(vmax - vmin) * alpha;
  }

  /* Get a random number between 0 and std::numeric_limts<T>::max() */
  template <typename T> T random () {
    return random<f64_t>() * std::numeric_limits<T>::max();
  }

  /* Get a random 128 bit floating point number between 0 and 1 */
  template <> f128_t random () {
    return static_cast<f128_t>(rand()) / static_cast<f128_t>(RAND_MAX);
  }

  /* Get a random 64 bit floating point number between 0 and 1 */
  template <> f64_t random () {
    return static_cast<f64_t>(rand()) / static_cast<f64_t>(RAND_MAX);
  }
  
  /* Get a random 32 bit floating point number between 0 and 1 */  
  template <> f32_t random () {
    return static_cast<f32_t>(rand()) / static_cast<f32_t>(RAND_MAX);
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


  /* Convert radians to degrees */
  template <typename T, typename U = std::conditional_t<std::is_floating_point_v<T>, T, f64_t>> constexpr U rad_to_deg (T rad) {
    return U(rad) * (U(180) / U(M_PI));
  }


  /* Convert degrees to radians */
  template <typename T, typename U = std::conditional_t<std::is_floating_point_v<T>, T, f64_t>> constexpr U deg_to_rad (T deg) {
    return U(deg) * (U(M_PI) / U(180));
  }
}


namespace mod {
  template <size_t in_max_length = 64> struct Name {
    static constexpr size_t max_length = in_max_length;

    char value [max_length];

    Name (char const* in_value = NULL) {
      *this = in_value;
    }

    Name& operator = (char const* in_value) {
      size_t len = in_value != NULL? num::min(strlen(in_value), max_length) : 0;
      if (len != 0) memory::copy(value, in_value, len);
      value[len] = '\0';
      return *this;
    }

    operator char* () const {
      return const_cast<char*>(value);
    }

    operator char const* () const {
      return value;
    }
  };

  
  namespace memory {
    ENGINE_API extern size_t allocated_size;
    ENGINE_API extern size_t allocation_count;

    #ifdef MEMORY_DEBUG_INDEPTH
      using AllocationName = Name<128>;
      using AllocationTrace = Name<1024>;

      struct AllocationTracePair {
        AllocationTrace origin = { };
        AllocationTrace realloc = { };
      };
      
      ENGINE_API extern void** allocated_addresses;
      ENGINE_API extern AllocationName* allocated_types;
      ENGINE_API extern AllocationTracePair* allocated_traces;
      ENGINE_API extern size_t* allocated_type_sizes;
      ENGINE_API extern size_t allocated_address_count;
      ENGINE_API extern size_t allocated_address_capacity;
      
      template <typename T> void register_address (T* address, char const* trace) {
        size_t new_count = allocated_address_count + 1;
        size_t new_capacity = allocated_address_capacity;
        while (new_capacity < new_count) new_capacity *= 2;
        if (new_capacity > allocated_address_capacity) {
          allocated_addresses = reinterpret_cast<void**>(realloc(allocated_addresses, new_capacity * sizeof(void*)));
          allocated_types = reinterpret_cast<AllocationName*>(realloc(allocated_types, new_capacity * sizeof(AllocationName)));
          allocated_traces = reinterpret_cast<AllocationTracePair*>(realloc(allocated_traces, new_capacity * sizeof(AllocationTracePair)));
          allocated_type_sizes = reinterpret_cast<size_t*>(realloc(allocated_type_sizes, new_capacity * sizeof(size_t)));
          allocated_address_capacity = new_capacity;
        }
        allocated_addresses[allocated_address_count] = address;
        allocated_types[allocated_address_count] = typeid(T).name();
        new (allocated_traces + allocated_address_count) AllocationTracePair { };
        allocated_traces[allocated_address_count].origin = trace;
        if constexpr (std::is_same_v<void, T>) allocated_type_sizes[allocated_address_count] = 1;
        else allocated_type_sizes[allocated_address_count] = sizeof(T);
        ++ allocated_address_count;
      }

      static void update_address (void* old_address, void* new_address, char const* trace) {
        for (size_t i = 0; i < allocated_address_count; i ++) {
          if (allocated_addresses[i] == old_address) {
            allocated_addresses[i] = new_address;
            allocated_traces[i].realloc = trace;
            break;
          }
        }
      }

      static void unregister_address (void* address) {
        for (size_t i = 0; i < allocated_address_count; i ++) {
          if (allocated_addresses[i] == address) {
            size_t index = i;

            -- allocated_address_count;

            while (index < allocated_address_count) {
              allocated_addresses[index] = allocated_addresses[index + 1];
              allocated_types[index] = allocated_types[index + 1];
              allocated_traces[index] = allocated_traces[index + 1];
              allocated_type_sizes[index] = allocated_type_sizes[index + 1];
              ++ index;
            }

            return;
          }
        }

        m_error("Cannot unregister unknown address %p", address);
      }
    #endif
    

    template <typename T, void* (*allocator) (size_t) = malloc> T* allocate_untracked (size_t size, bool clear) {
      m_assert(size != 0, "Cannot allocate zero sized buffer");

      if constexpr (!std::is_same_v<T, void>) size *= sizeof(T);

      auto mem = reinterpret_cast<T*>(allocator(size));
      m_assert(mem != NULL, "Out of memory");

      if (clear) memset(mem, 0, size);

      return mem;
    }

    template <typename T, void* (*allocator) (size_t) = malloc> T* allocate_tracked (size_t size, bool clear) {
      m_assert(size != 0, "Cannot allocate zero sized buffer");

      if constexpr(!std::is_same_v<T, void>) size *= sizeof(T);

      size_t a_size = size + sizeof(size_t);

      auto mem = reinterpret_cast<size_t*>(allocator(a_size));
      m_assert(mem != NULL, "Out of memory");
      
      *(mem ++) = size;

      allocated_size += size;
      ++ allocation_count;

      // printf("New alloc size/count: %zu / %zu\n", allocated_size, allocation_count);

      if (clear) memset(mem, 0, size);

      T* ptr = reinterpret_cast<T*>(mem);

      #ifdef MEMORY_DEBUG_INDEPTH
        register_address(ptr, StringStackWalker.CreateStackStr());
      #endif

      return ptr;
    }


    template <typename T, bool tracked = true, void* (*allocator) (size_t) = malloc> T* allocate (size_t size, bool clear = false) {
      if constexpr (tracked) {
        #ifdef MEMORY_DEBUG_INDEPTH
          StringStackWalker.ShowCallstack();
        #endif

        return allocate_tracked<T, allocator>(size, clear);
      } else return allocate_untracked<T, allocator>(size, clear);
    }

    template <typename T, void* (*allocator) (size_t) = malloc> T* allocate (bool tracked, size_t size, bool clear = false) {
      if (tracked) {
        #ifdef MEMORY_DEBUG_INDEPTH
          StringStackWalker.ShowCallstack();
        #endif

        return allocate_tracked<T, allocator>(size, clear);
      } else return allocate_untracked<T, allocator>(size, clear);
    }
    




    template <typename T, void* (*reallocator) (void*, size_t) = realloc> T* reallocate_untracked (T*& mem, size_t size) {
      m_assert(size != 0, "Cannot reallocate zero sized buffer");

      if constexpr(!std::is_same_v<T, void>) size *= sizeof(T);

      mem = reinterpret_cast<T*>(reallocator(mem, size));
      m_assert(mem != NULL, "Out of memory");

      return mem;
    }

    template <typename T, void* (*reallocator) (void*, size_t) = realloc> T* reallocate_tracked (T*& mem, size_t size) {
      m_assert(size != 0, "Cannot reallocate zero sized buffer");

      if constexpr(!std::is_same_v<T, void>) size *= sizeof(T);

      size_t a_size = size + sizeof(size_t);

      auto omem = reinterpret_cast<size_t*>(mem) - 1;
      size_t o_size = *omem;

      if (o_size > size) {
        allocated_size -= o_size - size;
      } else {
        allocated_size += size - o_size;
      }
      
      omem = reinterpret_cast<size_t*>(reallocator(omem, a_size));
      m_assert(omem != NULL, "Out of memory");

      *(omem ++) = size;

      T* new_mem = reinterpret_cast<T*>(omem);

      #ifdef MEMORY_DEBUG_INDEPTH
        update_address(mem, new_mem, StringStackWalker.CreateStackStr());
      #endif

      mem = new_mem;

      return mem;
    }


    template <typename T, bool tracked = true, void* (*reallocator) (void*, size_t) = realloc> T* reallocate (T*& mem, size_t size) {
      if constexpr (tracked) {
        #ifdef MEMORY_DEBUG_INDEPTH
          StringStackWalker.ShowCallstack();
        #endif

        return reallocate_tracked<T, reallocator>(mem, size);
      } else return reallocate_untracked<T, reallocator>(mem, size);
    }


    template <typename T, void* (*reallocator) (void*, size_t) = realloc> T* reallocate (bool tracked, T*& mem, size_t size) {
      if (tracked) {
        #ifdef MEMORY_DEBUG_INDEPTH
          StringStackWalker.ShowCallstack();
        #endif

        return reallocate_tracked<T, reallocator>(mem, size);
      } else return reallocate_untracked<T, reallocator>(mem, size);
    }

    



    template <typename T, void (*deallocator) (void*) = free> void deallocate_untracked_const (T* mem) {
      m_assert(mem != NULL, "Cannot deallocate NULL pointer");
      deallocator(mem);
    }

    template <typename T, void (*deallocator) (void*) = free> void deallocate_tracked_const (T* mem) {
      m_assert(mem != NULL, "Cannot deallocate NULL pointer");
      auto omem = reinterpret_cast<size_t*>(mem) - 1;
      allocated_size -= *omem;
      -- allocation_count;
      // printf("New alloc size/count: %zu / %zu\n", allocated_size, allocation_count);

      #ifdef MEMORY_DEBUG_INDEPTH
        unregister_address(mem);
      #endif

      deallocator(omem);
    }



    template <typename T, void (*deallocator) (void*) = free> void deallocate_untracked (T*& mem) {
      deallocate_untracked_const<T, deallocator>(mem);
      mem = NULL;
    }

    template <typename T, void (*deallocator) (void*) = free> void deallocate_tracked (T*& mem) {
      deallocate_tracked_const<T, deallocator>(mem);
      mem = NULL;
    }



    template <typename T, bool tracked = true, void (*deallocator) (void*) = free> void deallocate_const (T* mem) {
      if constexpr (tracked) return deallocate_tracked_const<T>(mem);
      else return deallocate_untracked_const<T, deallocator>(mem);
    }

    template <typename T, void (*deallocator) (void*) = free> void deallocate_const (bool tracked, T* mem) {
      if (tracked) return deallocate_tracked_const<T>(mem);
      else return deallocate_untracked_const<T, deallocator>(mem);
    }

    
    

    template <typename T, bool tracked = true, void (*deallocator) (void*) = free> void deallocate (T*& mem) {
      if constexpr (tracked) return deallocate_tracked<T>(mem);
      else return deallocate_untracked<T, deallocator>(mem);
    }

    template <typename T, void (*deallocator) (void*) = free> void deallocate (bool tracked, T*& mem) {
      if (tracked) return deallocate_tracked<T>(mem);
      else return deallocate_untracked<T, deallocator>(mem);
    }

    



    template <typename T> size_t get_tracked_allocation_size (T* mem) {
      return *(reinterpret_cast<size_t*>(mem) - 1);
    }

    template <typename T> size_t get_tracked_allocation_element_count (T* mem) {
      return get_tracked_allocation_size(mem) / sizeof(T);
    }

    static size_t get_tracked_allocation_element_count (void* mem, size_t type_size) {
      return get_tracked_allocation_size(mem) / type_size;
    }

    

    static void dump_allocation_data (FILE* stream = stdout) {
      fprintf(stream, "Allocation count %zu, total allocation size %zu\n", allocation_count, allocated_size);
      #ifdef MEMORY_DEBUG_INDEPTH
      for (size_t i = 0; i < allocated_address_count; i ++) {
        fprintf(stream, "%p : %s\n", allocated_addresses[i], allocated_types[i].value);
        fprintf(stream, "- Allocation stack trace:\n%s\n", allocated_traces[i].origin.value);
        if (allocated_traces[i].realloc.value[0] != '\0') fprintf(stream, "- Last Reallocation stack trace:\n%s\n", allocated_traces[i].realloc.value);
        fprintf(stream, "- Size %zu\n", get_tracked_allocation_size(allocated_addresses[i]));
        fprintf(stream, "- Count %zu\n", get_tracked_allocation_element_count(allocated_addresses[i], allocated_type_sizes[i]));
      }
      #endif
    }



    template <typename T, typename U> T* copy (T* destination, U const* source, size_t size = 1) {
      if constexpr (!std::is_same_v<T, void>) size *= sizeof(T);
      memcpy(destination, source, size);
      return destination;
    }

    
    template <typename T, typename U> T* move (T* destination, U const* source, size_t size = 1) {
      if constexpr (!std::is_same_v<T, void>) size *= sizeof(T);
      memmove(destination, source, size);
      return destination;
    }


    template <typename T> T* set (T* data, T const& value, size_t size = 1) {
      for (size_t i = 0; i < size; i ++) copy(data + i, &value);
    }


    template <typename T> T* clear (T* data, size_t size = 1) {
      if constexpr (!std::is_same_v<T, void>) size *= sizeof(T);
      memset(data, 0, size);
      return data;
    }


    template <typename T> T& clear (T& data) {
      return *clear(&data);
    }

    template <typename T> T cleared () {
      T value;
      clear(value);
      return value;
    }
  }

  /* Wrapper for snprintf that produces a new heap-allocated str */
  static char* str_fmt_va (char const* fmt, va_list args) {
    va_list args_copy;

    va_copy(args_copy, args);

    int length = vsnprintf_nonliteral(NULL, 0, fmt, args_copy) + 1;

    va_end(args_copy);

    auto out = memory::allocate<char>(length, true);

    vsnprintf_nonliteral(out, length, fmt, args);

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
    
    auto out = memory::allocate<char>(length + 1);

    m_assert(out != NULL, "Out of memory or other null pointer error while allocating new str via str_clone with length %zu + 1", length);
    
    memory::copy(out, base, length);

    out[length] = '\0';

    return out;
  }
}



/* Determine the length of an array */
#define m_array_length(ARR) (sizeof(ARR) / sizeof(ARR[0]))

/* Get the address offset of a structure's field */
#define m_field_offset(TY, FIELD) ((size_t) (&((TY*) 0)->FIELD))




#endif