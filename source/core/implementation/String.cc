#include "../include/String.hh"
#include "../include/Exception.hh"



namespace mod {
  String::String (char const* in_value, size_t in_length, bool in_is_static)
  : is_static(in_is_static)
  {
    if (in_length == 0) in_length = strlen(in_value);

    size_t in_capacity = default_capacity;

    while (in_capacity < in_length + 1) in_capacity *= 2;

    value = memory::allocate<char>(!is_static, in_capacity);

    memory::copy(value, in_value, in_length);
    value[in_length] = '\0';

    length = in_length;
    capacity = in_capacity;
  }

  String String::from_ex (char* value, size_t length, bool is_static) {
    if (length == 0) length = strlen(value);

    size_t capacity = default_capacity;

    while (capacity < length + 1) capacity *= 2;

    if (is_static) {
      char* new_mem = memory::allocate<char, false>(capacity);
      memory::copy(new_mem, value, length);
      memory::deallocate(value);
      value = new_mem;
    } else {
      memory::reallocate(value, capacity);
    }

    value[length] = '\0';

    return String { value, length, capacity, is_static };
  }

  String String::from_file (char const* path, bool is_static) {
    auto [ value, length ] = load_file(path);
    m_asset_assert(value != NULL, path, "Failed to load String from file");
    return from_ex(static_cast<char*>(value), length, is_static);
  }

  void String::destroy () {
    if (value != NULL) memory::deallocate(!is_static, value);

    length = 0;
    capacity = 0;
  }

  void String::grow_allocation (size_t additional_length) {
    size_t new_length = length + additional_length + 1; // account for null terminator

    size_t new_capacity = capacity != 0? capacity : default_capacity;

    while (new_length > new_capacity) {
      new_capacity *= 2;
    }

    if (new_capacity != capacity) {
      if (value != NULL) memory::reallocate(!is_static, value, new_capacity);
      else value = memory::allocate<char>(!is_static, new_capacity);

      m_assert(value != NULL, "Out of memory or other null pointer error while reallocating String for capacity %zu", new_capacity);

      capacity = new_capacity;
    }
  }

  void String::reallocate (size_t new_length) {
    if (new_length > length) grow_allocation(new_length - length);
  }

  void String::set_value (char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    if (new_value_length > length) {
      grow_allocation(new_value_length - length);
    }

    memory::move(value, new_value, new_value_length);
    length = new_value_length;
    value[length] = 0;
  }

  void String::append (char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    grow_allocation(new_value_length);

    memory::move(value + length, new_value, new_value_length);
    length += new_value_length;
    value[length] = 0;
  }

  void String::insert (size_t offset, char const* new_value, size_t new_value_length) {
    if (new_value_length == 0) new_value_length = strlen(new_value);

    grow_allocation(new_value_length);

    if (offset >= length) return append(new_value, new_value_length);
    
    memory::move(value + offset + new_value_length, value + offset, length - offset);
    memory::move(value + offset, new_value, new_value_length);
    length += new_value_length;
    value[length] = 0;
  }

  void String::remove (size_t offset, size_t remove_length) {
    size_t end = offset + remove_length;
    if (end > length) return;

    memory::move(value + offset, value + offset + remove_length, length - end);

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
    
    memory::move(end, start, length - offset);

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

  bool String::starts_with (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length >= other_value_length && strncmp(value, other_value, other_value_length) == 0;
  }

  bool String::ends_with (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length >= other_value_length && strncmp(value + length - other_value_length, other_value, other_value_length) == 0;
  }

  bool String::starts_with (String const& other) const {
    return starts_with(other.value, other.length);
  }

  bool String::ends_with (String const& other) const {
    return ends_with(other.value, other.length);
  }

  bool String::equal (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length == other_value_length && strncmp(value, other_value, other_value_length) == 0;
  }

  bool String::equal (String const* other_string) const {
    return equal(other_string->value, other_string->length);
  }

  bool String::not_equal (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length != other_value_length || strncmp(value, other_value, other_value_length) != 0;
  }

  bool String::not_equal (String const* other_string) const {
    return not_equal(other_string->value, other_string->length);
  }

  bool String::equal_caseless (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length == other_value_length && str_cmp_caseless(value, other_value, other_value_length) == 0;
  }

  bool String::equal_caseless (String const* other_string) const {
    return equal_caseless(other_string->value, other_string->length);
  }

  bool String::not_equal_caseless (char const* other_value, size_t other_value_length) const {
    if (other_value_length == 0) other_value_length = strlen(other_value);
    return length != other_value_length || str_cmp_caseless(value, other_value, other_value_length) != 0;
  }

  bool String::not_equal_caseless (String const* other_string) const {
    return not_equal_caseless(other_string->value, other_string->length);
  }
}