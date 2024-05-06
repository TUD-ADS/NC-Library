/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include "vector_adapter.h"

template <class Element>
internal::rr_vector_adapter<Element>::rr_vector_adapter(
    abstract_reconf_region_base &rr,
    sc_core::sc_vector<Element> &exposed_vector,
    const sc_core::sc_module_name &nm)
    : m_rr(rr), m_exposed_vector(exposed_vector) {}

template <class Element>
void internal::rr_vector_adapter<Element>::before_end_of_elaboration() {
  // initialize vector of adapters using the elements form exposed_vector
  m_adapter_vector.init(
      m_exposed_vector.size(), [&](const char *cname, size_t i) {
        return new rr_element_adapter_t(m_rr, m_exposed_vector[i]);
      });
}

template <class Element>
void internal::rr_vector_adapter<Element>::dynamic_bind(
    module_vector_adapter<Element> &module_adapter, bool sync) {
  // bind all of our elements to the respective elements in the module adapter
  // vector
  sc_assert(m_adapter_vector.size() == module_adapter.m_adapter_vector.size());

  // set up combined synced event first
  for (int i = 0; i < m_adapter_vector.size(); i++) {
    all_synced &= m_adapter_vector[i].synced();
  }

  for (int i = 0; i < m_adapter_vector.size(); i++) {
    m_adapter_vector[i].dynamic_bind(module_adapter.m_adapter_vector[i], sync);
  }
}

template <class Element>
void internal::rr_vector_adapter<Element>::dynamic_unbind(
    module_vector_adapter<Element> &module_adapter) {
  // unbind all of our elements to the respective elements in the module adapter
  // vector
  sc_assert(m_adapter_vector.size() == module_adapter.m_adapter_vector.size());

  // tear down combined synced event
  all_synced = sc_core::sc_event_and_list();

  for (int i = 0; i < m_adapter_vector.size(); i++) {
    m_adapter_vector[i].dynamic_unbind(module_adapter.m_adapter_vector[i]);
  }
}

template <class Element>
auto &internal::rr_vector_adapter<Element>::synced() const {
  return all_synced;
}

template <class Element>
internal::module_vector_adapter<Element>::module_vector_adapter(
    module_vector_adapter::module_exposed_t &exposed_vector,
    const sc_core::sc_module_name &nm)
    : m_exposed_vector(exposed_vector) {}

template <class Element>
void internal::module_vector_adapter<Element>::before_end_of_elaboration() {
  // initialize vector of adapters using the elements form exposed_vector
  m_adapter_vector.init(
      m_exposed_vector.size(), [this](const char *cname, size_t i) {
        return new module_element_adapter_t(m_exposed_vector[i]);
      });
}