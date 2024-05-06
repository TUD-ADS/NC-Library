/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef FORWARDING_NC_DECOUPLED_VALUE_H_
#define FORWARDING_NC_DECOUPLED_VALUE_H_

#include <systemc>
#include <type_traits>

namespace internal {

/**
 * Empty struct with template argument T. Its specializations are expected to
 * specify a static function "get_value()", which returns the value that the
 * adapters in adapter_trait<T> should expose when they are decoupled.
 *
 * @tparam T the template argument which shall be specialized to the types of
 * the adapted members
 */
template <typename T>
struct nc_decoupled_value {};

// template<typename T, typename =
// std::enable_if_t<std::is_arithmetic<T>::value>> class
// nc_decoupled_value<sc_core::sc_out<T>> {
//     T value = 0;
// };

/**
 * Specialize nc_decoupled_value<T> where T is sc_out<type>. "get_value()"
 * always returns 0.
 */
#define NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(type)  \
  template <>                                        \
  struct nc_decoupled_value<sc_core::sc_out<type>> { \
    static type get_value() { return 0; }            \
  }

NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(bool);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(char);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(char16_t);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(char32_t);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(wchar_t);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(short);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(unsigned short);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(int);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(unsigned int);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(long);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(unsigned long);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(long long);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(unsigned long long);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(float);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(double);
NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC(long double);

#undef NC_INTERNAL_DECOUPLE_VALUE_ARITHMETIC

}  // namespace internal

#endif  // FORWARDING_NC_DECOUPLED_VALUE_H_
