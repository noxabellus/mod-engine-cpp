#ifndef BITMASK_H
#define BITMASK_H

#include "cstd.hh"
#include "util.hh"


namespace mod {
  template <size_t in_bit_count = 8>
  struct Bitmask {
    static_assert(in_bit_count % 8 == 0, "Bitmask must have a bit count evenly divisible by 8");

    static constexpr size_t bit_count = in_bit_count;
    static constexpr size_t byte_count = bit_count / 8;


    u8_t bytes [byte_count];


    /* Create a new zero-initialized Bitmask */
    Bitmask ()
    : bytes { }
    { }

    /* Create a new Bitmask from an initializer list of flag indices */
    template <typename ... A> Bitmask (A ... args)
    : Bitmask { }
    {
      set_multiple(args...);
    }
    

    /* Create a new Bitmask from a buffer of flag indices */
    static Bitmask from_indices (size_t const* indices, size_t index_count) {
      Bitmask mask;
      mask.set_multiple(indices, index_count);
      return mask;
    }


    /* Set all flags of a Bitmask to 0 */
    void clear () {
      memset(bytes, 0, byte_count);
    }
    

    /* Bitwise OR two Bitmasks together */
    Bitmask operator | (Bitmask const& r) const {
      return bor(r);
    }

    /* Bitwise AND two Bitmasks together */
    Bitmask operator & (Bitmask const& r) const {
      return band(r);
    }
    
    /* Bitwise XOR two Bitmasks together */
    Bitmask operator ^ (Bitmask const& r) const {
      return bxor(r);
    }

    /* Bitwise NOT a Bitmask */
    Bitmask operator ~ () const {
      return bnot();
    }


    /* Bitwise OR two Bitmasks together */
    Bitmask& operator |= (Bitmask const& r) {
      *this = bor(r);
      return *this;
    }

    /* Bitwise AND two Bitmasks together */
    Bitmask& operator &= (Bitmask const& r) {
      *this = band(r);
      return *this;
    }
    
    /* Bitwise XOR two Bitmasks together */
    Bitmask& operator ^= (Bitmask const& r) {
      *this = bxor(r);
      return *this;
    }

    /* Get an index of a Bitmask as a bool */
    bool operator [] (size_t index) const {
      return match_index(index);
    }


    /* Compare two Bitmasks */
    bool operator == (Bitmask const& r) const {
      return match_exact(r);
    }

    /* Compare two Bitmasks */
    bool operator != (Bitmask const& r) const {
      for (size_t i = 0; i < byte_count; i ++) {
        if (bytes[i] != r.bytes[i]) return true;
      }

      return false;
    }


    /* Enable a specific bit index of a Bitmask */
    void set (size_t index) {
      m_assert(index < bit_count, "Cannot set out of range Bitmask index %zu, valid range is 0 - %zu", index, bit_count - 1);
      bytes[index / 8] |= (1_u8 << (index % 8));
    }

    /* Disable a specific bit index of a Bitmask */
    void unset (size_t index) {
      m_assert(index < bit_count, "Cannot unset out of range Bitmask index %zu, valid range is 0 - %zu", index, bit_count - 1);
      bytes[index / 8] &= (~(1_u8 << (index % 8)));
    }

    /* Toggle a specific bit index of a Bitmask */
    void toggle (size_t index) {
      m_assert(index < bit_count, "Cannot toggle out of range Bitmask index %zu, valid range is 0 - %zu", index, bit_count - 1);
      bytes[index / 8] ^= (1_u8 << (index % 8));
    }


    /* Enable multiple bit indices of a Bitmask by iterating a buffer of indices */
    void set_list (size_t const* indices, size_t index_count) {
      for (size_t i = 0; i < index_count; i ++) set(indices[i]);
    }

    /* Disable multiple bit indices of a Bitmask by iterating a buffer of indices */
    void unset_list (size_t const* indices, size_t index_count) {
      for (size_t i = 0; i < index_count; i ++) unset(indices[i]);
    }

    /* Toggle multiple bit indices of a Bitmask by iterating a buffer of indices */
    void toggle_list (size_t const* indices, size_t index_count) {
      for (size_t i = 0; i < index_count; i ++) toggle(indices[i]);
    }


    /* Enable multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void set_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      size_t indices [arg_count] = { ((size_t) args)... };
      set_list(indices, arg_count);
    }

    /* Disable multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void unset_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      size_t indices [arg_count] = { ((size_t) args)... };
      unset_list(indices, arg_count);
    }

    /* Toggle multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void toggle_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      size_t indices [arg_count] = { ((size_t) args)... };
      toggle_list(indices, arg_count);
    }

    
    /* Bitwise OR two Bitmasks together */
    Bitmask bor (Bitmask const& r) const {
      Bitmask o;

      for (size_t i = 0; i < byte_count; i ++) {
        o.bytes[i] = (bytes[i] | r.bytes[i]);
      }

      return o;
    }

    /* Bitwise AND two Bitmasks together */
    Bitmask band (Bitmask const& r) const {
      Bitmask o;

      for (size_t i = 0; i < byte_count; i ++) {
        o.bytes[i] = (bytes[i] & r.bytes[i]);
      }

      return o;
    }

    /* Bitwise XOR two Bitmasks together */
    Bitmask bxor (Bitmask const& r) const {
      Bitmask o;

      for (size_t i = 0; i < byte_count; i ++) {
        o.bytes[i] = (bytes[i] ^ r.bytes[i]);
      }

      return o;
    }

    /* Bitwise NOT a Bitmask */
    Bitmask bnot () const {
      Bitmask o;

      for (size_t i = 0; i < byte_count; i ++) {
        o.bytes[i] = ~(bytes[i]);
      }

      return o;
    }
  

    /* Determine if a specific bit index is enabled for a Bitmask */
    bool match_index (size_t index) const {
      m_assert(index < bit_count, "Cannot match out of range Bitmask index %zu, valid range is 0 - %zu", index, bit_count - 1);

      u8_t m = 1 << (index % 8);

      return (bytes[index / 8] & m) == m;
    }

    /* Determine if any bits are enabled for a Bitmask */
    bool match_any () const {
      for (size_t i = 0; i < byte_count; i ++) {
        if (bytes[i] != 0) return true;
      }

      return false;
    }

    /* Determine if another Bitmask is a subset of the caller instance */
    bool match_subset (Bitmask const& r) const {
      for (size_t i = 0; i < byte_count; i ++) {
        if ((bytes[i] & r.bytes[i]) != r.bytes[i]) return false;
      }

      return true;
    }

    /* Determine if two Bitmasks are exactly the same */
    bool match_exact (Bitmask const& r) const {
      for (size_t i = 0; i < byte_count; i ++) {
        if (bytes[i] != r.bytes[i]) return false;
      }

      return true;
    }


    /* Print the indices of the enabled bits of a Bitmask */
    void print (FILE* stream = stdout) const {
      fprintf(stream, "Bitmask<%zu> {", byte_count);
      for (size_t i = 0; i < bit_count; i ++) {
        if (match_index(i)) fprintf(stream, " %zu", i);
      }
      fprintf(stream, " }");
    }

    /* Print the indices of the enabled bits of a Bitmask, followed by a newline character */
    void print_ln (FILE* stream = stdout) const {
      print(stream);
      fprintf(stream, "\n");
    }


    /* Print a full Bitmask as binary */
    void print_binary (FILE* stream = stdout) const {
      fprintf(stream, "Bitmask<%zu> (Binary) {", byte_count);
      for (size_t i = 0; i < bit_count; i ++) {
        if (match_index(i)) fprintf(stream, "1");
        else fprintf(stream, "0");
      }
      fprintf(stream, " }");
    }

    /* Print a full Bitmask as binary, followed by a newline character */
    void print_binary_ln (FILE* stream = stdout) const {
      print_binary(stream);
      fprintf(stream, "\n");
    }
  };
}

#endif