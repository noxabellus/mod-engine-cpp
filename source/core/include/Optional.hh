#ifndef OPTIONAL_H
#define OPTIONAL_H

#include "cstd.hh"
#include "Exception.hh"



namespace mod {
  template <typename T, bool use_exception = false> struct Optional {
    using value_t = std::remove_reference_t<T>;
    using element_t = std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<value_t>, T>;

    bool valid;
    union {
      element_t value;
      size_t _void;
    };


    Optional ()
    : valid(false)
    , _void(0)
    { }

    Optional (T in_value)
    : valid(true)
    , value(in_value)
    { }


    Optional& operator = (T in_value) {
      valid = true;
      value = in_value;
      return *this;
    }


    value_t& unwrap () const {
      if constexpr (use_exception) {
        m_asset_assert(valid, "Optional", "Value of type %s not valid", typeid(value_t).name());
      } else {
        m_assert(valid, "Optional value of type %s not valid", typeid(value_t).name());
      }

      return const_cast<value_t&>(value);
    }

    template <typename ... A> value_t& unwrap (char const* fmt, A ... args) const {
      if constexpr (use_exception) {
        m_asset_assert(valid, "Optional", fmt, args...);
      } else {
        m_assert_nonliteral(valid, fmt, args...);
      }

      return const_cast<value_t&>(value);
    }

    operator value_t& () const {
      return unwrap();
    }

    value_t& operator * () const {
      return unwrap();
    }

    value_t* operator -> () const {
      return &unwrap();
    }
  };
}

#endif