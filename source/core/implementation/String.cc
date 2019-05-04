#include "../include/String.hh"
#include "../include/Exception.hh"



namespace mod {
  String::String (char const* new_value, size_t new_length) {
    if (new_length == 0) new_length = strlen(new_value);

    size_t new_capacity = default_capacity;

    while (new_capacity < new_length + 1) new_capacity *= 2;

    value = static_cast<char*>(malloc(new_capacity));

    m_assert(value != NULL, "Out of memory or other null pointer error while allocating String value with capacity %zu", new_capacity);

    memcpy(value, new_value, new_length);
    value[new_length] = '\0';

    length = new_length;
    capacity = new_capacity;
  }

  String String::from_ex (char* value, size_t length) {
    if (length == 0) length = strlen(value);

    size_t capacity = default_capacity;

    while (capacity < length + 1) capacity *= 2;

    value = static_cast<char*>(realloc(value, capacity));

    m_assert(value != NULL, "Out of memory or other null pointer error while reallocating value for String from_ex with capacity %zu", capacity);

    value[length] = '\0';

    return String { value, length, capacity };
  }

  String String::from_file (char const* path) {
    auto [ value, length ] = load_file(path);
    m_asset_assert(value != NULL, path, "Failed to load String from file");
    return from_ex(static_cast<char*>(value), length);
  }


  void String::grow_allocation (size_t additional_length) {
    size_t new_length = length + additional_length + 1; // account for null terminator

    size_t new_capacity = capacity != 0? capacity : default_capacity;

    while (new_length > new_capacity) {
      new_capacity *= 2;
    }

    if (new_capacity != capacity) {
      value = static_cast<char*>(value != NULL? realloc(value, new_capacity) : malloc(new_capacity));

      m_assert(value != NULL, "Out of memory or other null pointer error while reallocating String for capacity %zu", new_capacity);

      capacity = new_capacity;
    }
  }

  void String::set_value (char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    if (new_value_length > length) {
      grow_allocation(new_value_length - length);
    }

    memmove(value, new_value, new_value_length);
    length = new_value_length;
    value[length] = 0;
  }

  void String::append (char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    grow_allocation(new_value_length);

    memmove(value + length, new_value, new_value_length);
    length += new_value_length;
    value[length] = 0;
  }

  void String::insert (size_t offset, char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    grow_allocation(new_value_length);

    if (offset >= length) return append(new_value, new_value_length);
    
    memmove(value + offset + new_value_length, value + offset, length - offset);
    memmove(value + offset, new_value, new_value_length);
    length += new_value_length;
    value[length] = 0;
  }

  void String::remove (size_t offset, size_t remove_length) {
    size_t end = offset + remove_length;
    if (end > length) return;

    memmove(value + offset, value + offset + remove_length, length - end);

    length -= remove_length;
    value[length] = 0;
  }


  void String::fmt_va (char const* fmt, va_list args) {
    va_list args_copy;

    va_copy(args_copy, args);

    size_t new_length = vsnprintf_nonliteral(NULL, 0, fmt, args_copy);

    va_end(args_copy);

    if (new_length > length) {
      grow_allocation(new_length - length);
    }
    
    vsnprintf_nonliteral(value, new_length + 1, fmt, args);

    length = new_length;
  }

  void String::fmt (char const* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fmt_va(fmt, args);
    va_end(args);
  }


  void String::fmt_append_va (char const* fmt, va_list args) {
    va_list args_copy;

    va_copy(args_copy, args);

    size_t new_value_length = vsnprintf_nonliteral(NULL, 0, fmt, args_copy);

    va_end(args_copy);

    grow_allocation(new_value_length);

    vsnprintf_nonliteral(value + length, new_value_length + 1, fmt, args);

    length += new_value_length;
  }

  void String::fmt_append (char const* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fmt_append_va(fmt, args);
    va_end(args);
  }


  void String::fmt_insert_va (size_t offset, char const* fmt, va_list args) {
    if (offset >= length) return fmt_append_va(fmt, args);

    va_list args_copy;

    va_copy(args_copy, args);

    size_t new_value_length = vsnprintf_nonliteral(NULL, 0, fmt, args_copy);

    va_end(args_copy);

    grow_allocation(new_value_length);

    char* end = value + offset + new_value_length;
    char* start = value + offset;
    
    memmove(end, start, length - offset);

    char c = *end; // store char overwritten by vsnprintf null terminator

    vsnprintf_nonliteral(start, new_value_length + 1, fmt, args);

    *end = c; // restore overwitten char

    length += new_value_length;
    
    value[length] = 0;
  }

  void String::fmt_insert (size_t offset, char const* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    fmt_insert_va(offset, fmt, args);
    va_end(args);
  }
}