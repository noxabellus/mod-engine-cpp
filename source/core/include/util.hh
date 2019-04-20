#ifndef UTIL_H
#define UTIL_H

#include "cstd.hh"



namespace mod {
  /* The number of bytes read from a file at a time when calling load_file */
  static constexpr size_t FILE_READ_CHUNK_SIZE = 1024 * 1024;

  /* Get the index offset of a pointer from the base address of it's buffer */
  static size_t pointer_to_index (void const* base, void const* instance, size_t size) {
    return ((size_t) instance - (size_t) base) / size;
  }

  /* Get the index offset of a pointer from the base address of its buffer */
  template<typename T> size_t pointer_to_index (T const* base, T const* instance) {
    return ((size_t) instance - (size_t) base) / sizeof(T);
  }

  /* Load a file from disk, returns NULL if the file could not be loaded */
  ENGINE_API pair_t<void*, size_t> load_file (char const* path);

  /* Save a file to disk, returns true if the file was successfully saved */
  ENGINE_API bool save_file (char const* path, void const* data, size_t size);

  /* Determine whether a file exists */
  ENGINE_API bool file_exists (char const* path);

  /* Get the length of the parent directory part of a file path str.
  * For example, for the path `./a/b/c.xyz`, this will return the length of the substring `./a/b`.
  * Returns -1 if no directory separators are found in the str */
  ENGINE_API s64_t str_dir_parent_length (char const* path, size_t max_length = SIZE_MAX);

  /* Get the length of the directory chain `back` count above the end of a file path str.
  * For example, for the path `./a/b/c.xyz`, a `back` count of 2 will return the length of the substring `./a`.
  * Returns -1 if no directory seperators are found in the str */ 
  ENGINE_API s64_t str_dir_traverse_back (char const* path, size_t back, size_t max_length = SIZE_MAX);

  /* Get the number of `..\` or `../` at the beginning of `path` */
  ENGINE_API size_t str_dir_count_back_paths (char const* path, size_t max_length = SIZE_MAX);

  /* Combine two path strs by traversing back along `base_path` for each `../` or `..\` of `relative_path`.
  * Additionally, removes a proceeding `./` or `.\` if either is present in `relative_path`.
  * On success, the result is placed in `out`, and `true` is returned.
  * Failure conditions: `base_path` is not deep enough to cover all back traversals, 
  * or `max_length` of the output is not long enough to contain the combined path */
  ENGINE_API bool str_dir_relativize_path (char const* base_path, char const* relative_path, char* out, size_t max_length = SIZE_MAX);

  /* Get the index of the start of the file extension part of a path,
  * E.G. The part after the last `.` if it is not followed by `\` or `/`.
  * Returns -1 if no `.` was found meeting the requirements */
  ENGINE_API s64_t str_file_extension (char const* str, size_t max_length = SIZE_MAX);


  /* Determine whether a str or subsection of a str `start` matches the beginning of `str` */
  ENGINE_API bool str_starts_with (char const* str, char const* start, size_t max_length = SIZE_MAX);

  /* Determine whether a str or subsection of a str `end` matches the end of `str` */
  ENGINE_API bool str_ends_with (char const* str, char const* end, size_t max_length = 0);

  /* Determine whether two strs are the same if case (a vs A) is disregarded
   * Returns:
   * -9999 if there are invalid arguments (one or both of the input strings is a NULL pointer).
   * < 0 if the first character that does not match has a lower value in str1 than in str2.
   * 0 if the contents of both strings are equal.
   * > 0 if the first character that does not match has a greater value in str1 than in str2 */
  ENGINE_API int str_cmp_caseless (char const* str1, char const* str2, size_t num = SIZE_MAX);


  /* Determine if an ASCII character is whitespace ' ', '\n', etc */
  static bool char_is_whitespace (char c) {
    return c == ' '
        || c == '\n'
        || c == '\t'
        || c == '\r';
  }

  /* Determine if an ASCII character is '0' thru '9' */
  static bool char_is_numeric (char c) {
    return c >= '0'
        && c <= '9';
  }


  /* A utility function to swap two elements */
  template <typename T> void swap (T* a, T* b) { 
    T t = *a; 
    *a = *b; 
    *b = t; 
  } 

    
  /* A generic quicksort implementation using a comparison callback */
  template <typename T, typename FN> void quick_sort(T* buffer, s64_t low, s64_t high, FN fn) {
    static const auto partition = [] (T* buffer, s64_t low, s64_t high, FN fn) -> s64_t {
      T const& pivot = buffer[high];

      s64_t i = (low - 1);

      for (s64_t j = low; j <= high- 1; j++) { 
        T const& item = buffer[j];
        
        if (fn(item, pivot)) {
          ++ i;
          swap(buffer + i, buffer + j);
        }
      }

      swap(buffer + i + 1, buffer + high);

      return i + 1;
    };

    if (low >= high) return;
    
    s64_t pi = partition(buffer, low, high, fn);

    quick_sort(buffer, low, pi - 1, fn);
    quick_sort(buffer, pi + 1, high, fn);
  }
}




/* Get the name of a GL_type enum as a str */
ENGINE_API char const* glGetTypeName (u32_t type);

/* Convert a name of a GL blending enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glBlendFactorFromStr (char const* value);

/* Convert a name of a GL depth function enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glDepthFactorFromStr (char const* value);

/* Convert a name of a GL front face enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glFaceFromStr (char const* value);

/* Convert a name of a GL vertex winding enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glWindFromStr (char const* value);

/* Convert a name of a GL texture wrapping enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glWrapFromStr (char const* value);

/* Convert a name of a GL texture filter enum to an integer value.
 * Returns -1 if the value is not a match for any enums in the category */
ENGINE_API s64_t glFilterFromStr (char const* value);

/* Get a str containing information about an OpenGL error */
ENGINE_API char const* glGetErrorMsg (GLenum err);

/* Catch any OpenGL errors, log their messages, and abort if any errors were caught */
void glCatchErrors () {
  GLenum err;
  bool ok = true;
  while ((err = glGetError()) != GL_NO_ERROR) {
    printf("Uncaught OpenGL error detected:\n%s", glGetErrorMsg(err));
    ok = false;
  }
  if (!ok) abort();
}

#endif