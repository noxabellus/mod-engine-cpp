#pragma once

#include "cstd.hh"
#include "util.hh"

namespace mod {
  template <typename T>
  struct ArrayIterator {
    T* elements;
    size_t index;

    ArrayIterator& operator ++ () { ++ index; return *this; }

    bool operator != (ArrayIterator const& other) const { return index != other.index; }

    pair_t<u64_t, T&> operator * () const { return { index, *(elements + index) }; }
  };

  template <typename T>
  struct Array {
    static constexpr size_t default_capacity = 128;

    T* elements = NULL;
    size_t count = 0;
    size_t capacity = 0;


    /* Create a new zero-initialized Array */
    Array () = default;

    /* Create a new Array with a specific capacity */
    Array (size_t new_capacity)
    : elements((T*) malloc(capacity * sizeof(T)))
    , count(0)
    , capacity(new_capacity)
    {
      m_assert(elements != NULL, "Out of memory or other null pointer error while allocating Array elements with capacity %zu", capacity);
    }

    /* Create a new Array with explicit initialization of all members */
    Array (T* new_elements, size_t new_count, size_t new_capacity)
    : elements(new_elements)
    , count(new_count)
    , capacity(new_capacity)
    { }

    /* Create a new Array from a parameter pack list of elements */
    template <typename ... A> static Array from_elements (A ... args) {
      T arg_arr [] = { args... };
      return { arg_arr, m_array_length(arg_arr) };
    }

    /* Create a new Array by copying an existing buffer or region */
    Array (T* new_elements, size_t new_count) {
      size_t new_capacity = default_capacity;

      while (new_capacity < new_count) new_capacity *= 2;

      elements = (T*) malloc(new_capacity * sizeof(T));

      m_assert(elements != NULL, "Out of memory or other null pointer error while allocating Array elements with capacity %zu", new_capacity);

      memcpy(elements, new_elements, new_count * sizeof(T));
      
      count = new_count;
      capacity = new_capacity;
    }


    /* Create a new Array by taking ownership of an existing buffer */
    static Array from_ex (T* elements, size_t count) {
      size_t capacity = default_capacity;

      while (capacity < count) capacity *= 2;

      elements = (T*) realloc(elements, capacity * sizeof(T));

      m_assert(elements != NULL, "Out of memory or other null pointer error while reallocating elements for Array from_ex with capacity %zu", capacity);

      return Array { elements, count, capacity };
    }


    /* Create an Array by copying it from a file. 
     * Throws an exception if the file could not be loaded or was an odd size */
    static Array from_file (char const* path) {
      auto [ elements, byte_length ] = load_file(path);
      m_asset_assert(elements != NULL, path, "Failed to load Array from file");
      size_t rem = byte_length % sizeof(T);
      m_asset_assert(rem == 0, path, "Failed to load Array from file: The file has an odd size, should be evenly divisible by %zu but size was %zu (remainder %zu)", sizeof(T), byte_length, rem);
      return from_ex(elements, byte_length / sizeof(T));
    }

    /* Create a new Array by cloning an existing Array */
    Array clone () const {
      return { elements, count };
    }


    /* Save an array to file. 
     * Returns true if the file was successfully saved */
    bool to_file (char const* path) const {
      return save_file(path, elements, count * sizeof(T));
    }

    
    /* Free the heap allocation of an Array */
    void destroy () {
      if (elements != NULL) free(elements);
    }


    /* Access a specific element of an Array */
    T& operator [] (size_t index) const {
      return *get_element(index);
    }


    /* Get an iterator representing the beginning of an Array */
    ArrayIterator<T> begin () const { return { elements, 0 }; }

    /* Get an iterator representing the end of an Array */
    ArrayIterator<T> end () const { return { elements, count }; }
    

    /* Iterate over an Array and call a callback closure for each element,
     * overwriting the element with the return value of the callback */
    template <typename FN> void map_in_place (FN fn) {
      for (auto [ i, v ] : *this) set_element(i, fn(v));
    }

    /* Iterate over an Array and call a callback closure for each element,
     * placing the return value of the callback into a new Array */
    template <typename U, typename FN> Array<U> map (FN fn) const {
      Array<U> out;
      for (auto [ i, v ] : *this) {
        T const& imm = v;
        out.append(fn(imm));
      }
      return out;
    }

    /* Iterate over an Array and call a callback closure for each element,
     * removing the element, if the callback returns a falsey value */
    template <typename FN> void filter_in_place (FN fn) {
      size_t i = 0;
      while (i < count) {
        T& value = (*this)[i];
        if (fn(value)) ++ i;
        else remove(i);
      }
    }

    /* Iterate over an Array and call a callback closure for each element,
     * placing the element into a new Array, if the callback returns a truethy value */
    template <typename FN> Array filter (FN fn) const {
      Array out;
      for (auto [ i, v ] : *this) {
        T const& imm = v;
        if(fn(imm)) out.append(imm);
      }
      return out;
    }

    /* Iterate over an Array and call a callback closure for each element,
     * using its return value to quicksort the Array in place */
    template <typename FN> void sort_in_place (FN fn) {
      quick_sort(elements, 0, count - 1, fn);
    }

    /* Iterate over an Array and call a callback closure for each element,
     * using its return value to quicksort the Array into a new Array */
    template <typename FN> Array sort (FN fn) const {
      Array out = clone();
      out.sort_in_place(fn);
      return out;
    }

    /* Iterate over an Array and call a callback closure for each element,
     * along with an additional accumulator value. Returns the final accumulator value */
    template <typename U, typename FN> U reduce (U accumulator, FN fn) const {
      U& acc_ref = accumulator;
      for (auto [ i, v ] : *this) {
        T const& imm = v;
        fn(acc_ref, imm);
      }
      return accumulator;
    }


    /* Grow the allocation of an Array (if necessary) to support some additional count of elements (Defaults to 1) */
    void grow_allocation (size_t additional_count = 1) {
      size_t new_count = count + additional_count;

      size_t new_capacity = capacity != 0? capacity : default_capacity;

      while (new_count > new_capacity) {
        new_capacity *= 2;
      }

      if (new_capacity != capacity) {
        size_t byte_size = new_capacity * sizeof(T);
        elements = (T*) (elements != NULL? realloc(elements, byte_size) : malloc(byte_size));

        m_assert(elements != NULL, "Out of memory or other null pointer error while reallocating Array for capacity %zu", new_capacity);

        capacity = new_capacity;
      }
    }


    /* Get a pointer to a specific element of an Array */
    T* get_element (size_t index) const {
      m_assert(index < count, "Out of range access for Array: index %zu, count %zu", index, count);
      return (T*) elements + index;
    }

    /* Set a specific element of an Array */
    void set_element (size_t index, T const* value) {
      *get_element(index) = *value;
    }

    /* Set a specific element of an Array, by reference */
    void set_element (size_t index, T const& value) { set_element(index, &value); }

    /* Append an element to the end of an Array */
    void append (T const* value) {
      grow_allocation();
      
      new (elements + count) T { *value };

      ++ count;
    }

    /* Append an element to the end of an Array, by reference */
    void append (T const& value) { return append(&value); }

    /* Insert an element at a designated point inside an Array */
    void insert (size_t index, T const* value) {
      if (index >= count) return append(value);

      grow_allocation();
      
      new (elements + count) T { elements[count - 1] };

      for (size_t i = count - 1; i > index; i --) elements[i] = elements[i - 1];

      *(elements + index) = *value;

      ++ count;
    }

    /* Insert an element at a designated point inside an Array, by reference */
    void insert (size_t index, T const& value) { return insert(index, &value); }

    /* Remove an element from an Array */
    void remove (size_t index) {
      if (index >= count) return;

      elements[index].~T();

      -- count;
      while (index < count) {
        elements[index] = elements[index + 1];
        ++ index;
      }
    }
  };
}