/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef FORWARDING_VECTOR_ADAPTER_H_
#define FORWARDING_VECTOR_ADAPTER_H_

#include "forwarding/adapter_traits.h"
#include "region/abstract_reconf_region_base.h"

namespace internal {

template <class Element>
class module_vector_adapter;

template <class Element>
class rr_vector_adapter;

///
/// adapter traits specialization
///
template <typename Element>
struct adapter_traits<sc_core::sc_vector<Element>> {
  typedef sc_core::sc_vector<Element> rr_exposed_t;
  typedef sc_core::sc_vector<Element> module_exposed_t;

  typedef internal::rr_vector_adapter<Element> rr_adapter_t;
  typedef internal::module_vector_adapter<Element> module_adapter_t;
};

///
/// rr-side adapter
///

template <class Element>
class rr_vector_adapter : public sc_core::sc_module {
  using rr_exposed_t = sc_core::sc_vector<Element>;
  using rr_element_adapter_t = typename adapter_traits<Element>::rr_adapter_t;

  abstract_reconf_region_base &m_rr;
  rr_exposed_t &m_exposed_vector;

  sc_core::sc_vector<rr_element_adapter_t> m_adapter_vector;
  sc_core::sc_event_and_list all_synced;

  /**
   * Get the default name for a sc_vector port adapter on the reconfiguration
   * region side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("module_vector_adapter");
  }

 protected:
  void before_end_of_elaboration() override;

 public:
  /**
   * TODO
   * Constructs an adapter given the reference to the reconfiguration region
   * that contains it, the adapted socket that is exposed by the reconfiguration
   * region and optionally a name.
   *
   * @param rr the reconfiguration region that contains the adapter
   * @param exposed_isock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */
  rr_vector_adapter(abstract_reconf_region_base &rr,
                    rr_exposed_t &exposed_vector,
                    const sc_core::sc_module_name &nm = default_name());

 public:
  /**
   * TODO
   * Couple the given module adapter with this adapter.
   * This sets up the forwarding procedures.
   * TLM socket adapters cannot synchronize anything.
   * Thus, if the passed sync flag is true, then the synced event will be
   * triggered in the next delta-cycle.
   *
   * @param module_adapter the module adapter that shall be coupled to this
   * adapter
   * @param sync if true, the synced event will be triggered in the next
   * delta-cycle
   */
  void dynamic_bind(module_vector_adapter<Element> &module_adapter,
                    bool sync = true);

  /**
   * TODO
   * Decouple the given module adapter from this adapter.
   * This tears down the forwarding procedures.
   *
   * @param module_adapter the module adapter that shall be decoupled from this
   * adapter
   */
  void dynamic_unbind(module_vector_adapter<Element> &module_adapter);

  /**
   * TODO
   * Retrieve the event that is triggered when synchronization completed.
   *
   * @return the synced event
   */
  auto &synced() const;
};

template <class Element>
class module_vector_adapter : public sc_core::sc_module {
  friend class rr_vector_adapter<Element>;

  using module_exposed_t = sc_core::sc_vector<Element>;
  using module_element_adapter_t =
      typename adapter_traits<Element>::module_adapter_t;

  module_exposed_t &m_exposed_vector;

  sc_core::sc_vector<module_element_adapter_t> m_adapter_vector;

  /**
   * Get the default name for a sc_vector port adapter on the module side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("module_vector_adapter");
  }

 protected:
  void before_end_of_elaboration() override;

 public:
  /**
   * TODO
   * Constructs an adapter given the adapted socket that is exposed by the
   * module and optionally a name.
   *
   * @param exposed_isock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */
  explicit module_vector_adapter(
      module_exposed_t &exposed_vector,
      const sc_core::sc_module_name &nm = default_name());
};

}  // namespace internal

#include "vector_adapter.tpp"

#endif  // FORWARDING_VECTOR_ADAPTER_H_
