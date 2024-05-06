/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef FORWARDING_ADAPTER_TRAITS_H_
#define FORWARDING_ADAPTER_TRAITS_H_

namespace internal {

/**
 * Helper struct to map adapters to their according adapted types.
 * The struct is templated with the type of the adapted member and contains
 * typedefs that specify the adapter type on for the module and the
 * reconfiguration region. There are also typedefs for the "exposed" type, which
 * refers to the member that is added to the module and reconfiguration region
 * respectively. Thus, it is for example possible to expose a channel at the
 * reconfiguration region and a port in the module. However, this is not used as
 * of now and the exposed types always correspond to the adapted type.
 *
 * @tparam T the type of the member variable that shall be adapted
 */
template <typename T>
struct adapter_traits {
  /// @brief type that is exposed by the reconfiguration region
  using rr_exposed_t = void;
  /// @brief type that is exposed by the module
  using module_exposed_t = void;

  /// @brief type of the adapter used by the reconfiguration region
  using rr_adapter_t = void;
  /// @brief type of the adapter used by the module
  using module_adapter_t = void;
};

// template<>
// struct adapter_traits<tlm_utils::simple_initiator_socket<>> {
//     // ...
// };
//
// template<>
// struct adapter_traits<tlm_utils::simple_target_socket<>> {
//     // ...
// };

// template<>
// struct adapter_traits<sc_core::sc_vector<tlm::tlm_initiator_socket<>>> {
//     // ...
// };
//
// template<>
// struct adapter_traits<sc_core::sc_vector<tlm::tlm_target_socket<>>> {
//     // ...
// };

}  // namespace internal

#endif  // FORWARDING_ADAPTER_TRAITS_H_
