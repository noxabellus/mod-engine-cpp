#pragma once

#include "cstd.hh"
#include "util.hh"


namespace mod {
  struct String {
    static constexpr size_t default_capacity = 32;

    char* value = NULL;
    size_t length = 0;
    size_t capacity = 0;


    /* Create a new zero-initialized String */
    String () = default;

    /* Create a new String with a specific capacity */
    String (size_t new_capacity)
    : value((char*) malloc(new_capacity))
    , length(0)
    , capacity(new_capacity)
    {
      m_assert(value != NULL, "Out of memory or other null pointer error while allocating String value with capacity %zu", capacity);
    }

    /* Create a new String with explicit initialization of all members */
    String (char* new_value, size_t new_length, size_t new_capacity)
    : value(new_value)
    , length(new_length)
    , capacity(new_capacity)
    { }

    /* Create a new String by copying an existing str or substr */
    ENGINE_API String (char const* new_value, size_t new_length = 0);

    /* Create a new String by taking ownership of an existing str */
    ENGINE_API static String from_ex (char* value, size_t length = 0);

    

    /* Create a String by copying it from a file.
     * Throws an exception if the file could not be loaded */
    static String from_file (char const* path) {
      auto [ value, length ] = load_file(path);
      m_asset_assert(value != NULL, path, "Failed to load String from file");
      return from_ex((char*) value, length);
    }

    /* Create a new String by cloning an existing String */
    String clone () {
      return { value, length };
    }


    /* Save a String to file. 
     * Returns true if the file was successfully saved */
    bool to_file (char const* path) const {
      return save_file(path, value, length);
    }


    /* Free the heap allocation of a String */
    void destroy () {
      free(value);
    }

    
    /* Access a specific char of a String */
    char& operator [] (size_t offset) const {
      m_assert(offset < length, "Out of range access for String: offset %zu, length %zu", offset, length);
      return value[offset];
    }

    /* Compare two Strings */
    bool operator == (String const& other) const { return equal(&other); }

    /* Compare two Strings */
    bool operator != (String const& other) const { return not_equal(&other); }

    /* Compare a String with a str */
    bool operator == (char const* other) const { return equal(other); }

    /* Compare a String with a str */
    bool operator != (char const* other) const { return not_equal(other); }


    /* Grow the allocation of a String (if necessary) to support some additional length (Defaults to 1) */
    ENGINE_API void grow_allocation (size_t additional_length = 1);


    /* Set the value of String by copying from a str or substr */
    ENGINE_API void set_value (char const* new_value, size_t new_value_length = 0);

    /* Add to the end of a String by copying from a str or substr */
    ENGINE_API void append (char const* new_value, size_t new_value_length = 0);

    /* Add to a designated point inside a String by rearranging and copying from a str or substr */
    ENGINE_API void insert (size_t offset, char const* new_value, size_t new_value_length = 0);

    /* Remove a section of a String */
    ENGINE_API void remove (size_t offset, size_t remove_length = 1);


    /* Overwrite a String with the product of vsnprintf */
    ENGINE_API void fmt_va (char const* fmt, va_list args);

    /* Overwrite a String with the product of snprintf */
    ENGINE_API void fmt (char const* fmt, ...);


    /* Append the product of vsnprintf to a String */
    ENGINE_API void fmt_append_va (char const* fmt, va_list args);

    /* Append the product of snprintf to a String */
    ENGINE_API void fmt_append (char const* fmt, ...);


    /* Add the product of vsnprintf to a designated point inside a String */
    ENGINE_API void fmt_insert_va (size_t offset, char const* fmt, va_list args);

    /* Add the product of snprintf to a designated point inside a String */
    ENGINE_API void fmt_insert (size_t offset, char const* fmt, ...);


    /* Compare a String to a str */
    bool equal (char const* other_value, size_t other_value_length = 0) const {
      if (other_value_length == 0) other_value_length = strlen(other_value);
      return length == other_value_length && strncmp(value, other_value, other_value_length) == 0;
    }

    /* Compare a String to another String */
    bool equal (String const* other_string) const {
      return equal(other_string->value, other_string->length);
    }

    /* Compare a String to a str */
    bool not_equal (char const* other_value, size_t other_value_length = 0) const {
      if (other_value_length == 0) other_value_length = strlen(other_value);
      return length != other_value_length || strncmp(value, other_value, other_value_length) != 0;
    }

    /* Compare a String to another String */
    bool not_equal (String const* other_string) const {
      return not_equal(other_string->value, other_string->length);
    }
  };
}