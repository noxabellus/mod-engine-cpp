#pragma once

#include "cstd.hh"
#include "util.hh"


namespace mod {
  template <u64_t element_count = 1>
  struct Bitmask {
    static constexpr u64_t byte_count = element_count * sizeof(u64_t);
    static constexpr u64_t bit_count  = byte_count * 8;

    union {
      u64_t elements [element_count];
      u8_t bytes [byte_count];
    };


    /* Create a new zero-initialized Bitmask */
    Bitmask ()
    : elements { }
    { }

    /* Create a new Bitmask from an initializer list of flag indices */
    template <typename ... A> Bitmask (A ... args)
    : Bitmask { }
    {
      set_multiple(args...);
    }
    

    /* Create a new Bitmask from a buffer of flag indices */
    static Bitmask from_indices (u64_t const* indices, u64_t index_count) {
      Bitmask mask;
      mask.set_multiple(indices, index_count);
      return mask;
    }


    /* Set all flags of a Bitmask to 0 */
    void clear () {
      memset(bytes, 0, byte_count);
    }
    

    /* Bitwise OR two Bitmasks together */
    Bitmask operator | (Bitmask const& r) { return bor(r); }

    /* Bitwise AND two Bitmasks together */
    Bitmask operator & (Bitmask const& r) { return band(r); }
    
    /* Bitwise XOR two Bitmasks together */
    Bitmask operator ^ (Bitmask const& r) { return bxor(r); }

    /* Bitwise NOT a Bitmask */
    Bitmask operator ~ () { return bnot(); }

    /* Compare two Bitmasks */
    bool operator == (Bitmask const& r) { return match_exact(r); }

    /* Compare two Bitmasks */
    bool operator != (Bitmask const& r) {
      for (u64_t i = 0; i < element_count; i ++) {
        if (elements[i] != r.elements[i]) return true;
      }

      return false;
    }


    /* Enable a specific bit index of a Bitmask */
    void set (u64_t index) {
      m_assert(index < bit_count, "Cannot set out of range Bitmask index %" PRIu64 ", valid range is 0 - %" PRIu64, index, bit_count - 1);
      bytes[index / 8] |= (1_u8 << (index % 8));
    }

    /* Disable a specific bit index of a Bitmask */
    void unset (u64_t index) {
      m_assert(index < bit_count, "Cannot unset out of range Bitmask index %" PRIu64 ", valid range is 0 - %" PRIu64, index, bit_count - 1);
      b->bytes[index / 8] &= (~(1_u8 << (index % 8)));
    }

    /* Toggle a specific bit index of a Bitmask */
    void toggle (u64_t index) {
      m_assert(index < bit_count, "Cannot toggle out of range Bitmask index %" PRIu64 ", valid range is 0 - %" PRIu64, index, bit_count - 1);
      b->bytes[index / 8] ^= (1_u8 << (index % 8));
    }


    /* Enable multiple bit indices of a Bitmask by iterating a buffer of indices */
    void set_list (u64_t const* indices, u64_t index_count) {
      for (u64_t i = 0; i < index_count; i ++) set(indices[i]);
    }

    /* Disable multiple bit indices of a Bitmask by iterating a buffer of indices */
    void unset_list (u64_t const* indices, u64_t index_count) {
      for (u64_t i = 0; i < index_count; i ++) unset(indices[i]);
    }

    /* Toggle multiple bit indices of a Bitmask by iterating a buffer of indices */
    void toggle_list (u64_t const* indices, u64_t index_count) {
      for (u64_t i = 0; i < index_count; i ++) toggle(indices[i]);
    }


    /* Enable multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void set_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      u64_t indices [arg_count] = { ((u64_t) args)... };
      set_list(indices, arg_count);
    }

    /* Disable multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void unset_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      u64_t indices [arg_count] = { ((u64_t) args)... };
      unset_list(indices, arg_count);
    }

    /* Toggle multiple bit indices of a Bitmask by iterating a parameter pack list of indices */
    template <typename ... A> void toggle_multiple (A ... args) {
      static constexpr size_t arg_count = sizeof...(args);
      u64_t indices [arg_count] = { ((u64_t) args)... };
      toggle_list(indices, arg_count);
    }

    
    /* Bitwise OR two Bitmasks together */
    Bitmask bor (Bitmask const& r) {
      Bitmask o;

      for (u64_t i = 0; i < element_count; i ++) {
        o.elements[i] = (elements[i] | r.elements[i]);
      }

      return o;
    }

    /* Bitwise AND two Bitmasks together */
    Bitmask band (Bitmask const& r) {
      Bitmask o;

      for (u64_t i = 0; i < element_count; i ++) {
        o.elements[i] = (elements[i] & r.elements[i]);
      }

      return o;
    }

    /* Bitwise XOR two Bitmasks together */
    Bitmask bxor (Bitmask const& r) {
      Bitmask o;

      for (u64_t i = 0; i < element_count; i ++) {
        o.elements[i] = (elements[i] ^ r.elements[i]);
      }

      return o;
    }

    /* Bitwise NOT a Bitmask */
    Bitmask bnot () {
      Bitmask o;

      for (u64_t i = 0; i < element_count; i ++) {
        o.elements[i] = ~(elements[i]);
      }

      return o;
    }
  

    /* Determine if a specific bit index is enabled for a Bitmask */
    bool match_index (u64_t index) {
      m_assert(index < bit_count, "Cannot match out of range Bitmask index %" PRIu64 ", valid range is 0 - %" PRIu64, index, bit_count - 1);

      u8_t m = 1 << (index % 8);

      return (bytes[index / 8] & m) == m;
    }

    /* Determine if any bits are enabled for a Bitmask */
    bool match_any () {
      for (u64_t i = 0; i < element_count; i ++) {
        if (elements[i] != 0) return true;
      }

      return false;
    }

    /* Determine if another Bitmask is a subset of the caller instance */
    bool match_subset (Bitmask const& r) {
      for (u64_t i = 0; i < element_count; i ++) {
        if ((elements[i] & r.elements[i]) != r.elements[i]) return false;
      }

      return true;
    }

    /* Determine if two Bitmasks are exactly the same */
    bool match_exact (Bitmask const& r) {
      for (u64_t i = 0; i < element_count; i ++) {
        if (elements[i] != r.elements[i]) return false;
      }

      return true;
    }


    /* Print the indices of the enabled bits of a Bitmask */
    void print (FILE* stream = stdout) {
      fprintf(stream, "Bitmask<%zu> {", element_count);
      for (u64_t i = 0; i < bit_count; i ++) {
        if (match_index(i)) fprintf(stream, " %" PRIu64, i);
      }
      fprintf(stream, " }");
    }

    /* Print the indices of the enabled bits of a Bitmask, followed by a newline character */
    void print_ln (FILE* stream = stdout) {
      print(stream);
      fprintf(stream, "\n");
    }


    /* Print a full Bitmask as binary */
    void print_binary (FILE* stream = stdout) {
      fprintf(stream, "Bitmask<%zu> (Binary) {", element_count);
      for (u64_t i = 0; i < bit_count; i ++) {
        if (match_index(i)) fprintf(stream, "1");
        else fprintf(stream, "0");
      }
      fprintf(stream, " }");
    }

    /* Print a full Bitmask as binary, followed by a newline character */
    void print_binary_ln (FILE* stream = stdout) {
      print_binary(stream);
      fprintf(stream, "\n");
    }
  };
}