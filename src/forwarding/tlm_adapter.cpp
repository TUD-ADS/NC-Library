/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include "tlm_adapter.h"

#include "locking/reconf_lock.h"
///
/// tlm_initiator_socket adapter
///
internal::rr_initiator_socket_adapter::rr_initiator_socket_adapter(
    abstract_reconf_region_base &rr, tlm::tlm_initiator_socket<> &exposed_isock,
    const sc_module_name &nm)
    : m_rr(rr),
      m_exposed_isock(exposed_isock),
      sc_module(sc_module_name(sc_gen_unique_name(nm))) {
  // bind this interface to the exposed socket
  m_exposed_isock.bind(*this);
}

tlm::tlm_sync_enum internal::rr_initiator_socket_adapter::nb_transport_bw(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_base_protocol_types::tlm_phase_type &phase, sc_time &t) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  return m_module_adapter->m_tsock->nb_transport_bw(trans, phase, t);
}

void internal::rr_initiator_socket_adapter::invalidate_direct_mem_ptr(
    sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  m_module_adapter->m_tsock->invalidate_direct_mem_ptr(start_range, end_range);
}

void internal::rr_initiator_socket_adapter::dynamic_bind(
    module_initiator_socket_adapter &module_adapter, bool sync /* = true */) {
  // make sure nothing is bound
  sc_assert(m_module_adapter == nullptr);
  m_module_adapter = &module_adapter;
  module_adapter.m_rr_adapter = this;

  if (sync) {
    // nothing to sync (this needs to notify with SC_ZERO_TIME, otherwise event
    // might be missed)
    m_synced.notify(sc_core::SC_ZERO_TIME);
  }
}

void internal::rr_initiator_socket_adapter::dynamic_unbind(
    module_initiator_socket_adapter &module_adapter) {
  // make sure we unbind from the correct socket
  sc_assert(m_module_adapter == &module_adapter);
  m_module_adapter = nullptr;
  module_adapter.m_rr_adapter = nullptr;
}

internal::module_initiator_socket_adapter::module_initiator_socket_adapter(
    tlm::tlm_initiator_socket<> &exposed_isock, const sc_module_name &nm)
    : sc_module(sc_module_name(sc_gen_unique_name(nm))),
      m_exposed_isock(exposed_isock) {
  m_tsock.bind(*this);
}

void internal::module_initiator_socket_adapter::before_end_of_elaboration() {
  m_tsock.bind(m_exposed_isock);
}

tlm::tlm_sync_enum internal::module_initiator_socket_adapter::nb_transport_fw(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_base_protocol_types::tlm_phase_type &phase, sc_time &t) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(m_rr_adapter->m_rr);
  return m_rr_adapter->m_exposed_isock->nb_transport_fw(trans, phase, t);
}

void internal::module_initiator_socket_adapter::b_transport(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans, sc_time &t) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(m_rr_adapter->m_rr);
  m_rr_adapter->m_exposed_isock->b_transport(trans, t);
}

bool internal::module_initiator_socket_adapter::get_direct_mem_ptr(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_dmi &dmi_data) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(m_rr_adapter->m_rr);
  return m_rr_adapter->m_exposed_isock->get_direct_mem_ptr(trans, dmi_data);
}

unsigned int internal::module_initiator_socket_adapter::transport_dbg(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(m_rr_adapter->m_rr);
  return m_rr_adapter->m_exposed_isock->transport_dbg(trans);
}

///
/// tlm_target_socket adapter
///

internal::rr_target_socket_adapter::rr_target_socket_adapter(
    abstract_reconf_region_base &rr, tlm::tlm_target_socket<> &exposed_tsock,
    const sc_module_name &nm)
    : m_rr(rr),
      m_exposed_tsock(exposed_tsock),
      sc_module(sc_module_name(sc_gen_unique_name(nm))) {
  // bind this interface to the exposed socket
  exposed_tsock.bind(*this);
}

tlm::tlm_sync_enum internal::rr_target_socket_adapter::nb_transport_fw(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_base_protocol_types::tlm_phase_type &phase, sc_time &t) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  return m_module_adapter->m_isock->nb_transport_fw(trans, phase, t);
}

void internal::rr_target_socket_adapter::b_transport(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans, sc_time &t) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  m_module_adapter->m_isock->b_transport(trans, t);
}

bool internal::rr_target_socket_adapter::get_direct_mem_ptr(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_dmi &dmi_data) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  return m_module_adapter->m_isock->get_direct_mem_ptr(trans, dmi_data);
}

unsigned int internal::rr_target_socket_adapter::transport_dbg(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans) {
  reconf_lock lock(m_rr);
  sc_assert(m_module_adapter != nullptr);
  return m_module_adapter->m_isock->transport_dbg(trans);
}

void internal::rr_target_socket_adapter::dynamic_bind(
    module_target_socket_adapter &module_adapter, bool sync /* = true */) {
  // make sure nothing is bound
  sc_assert(m_module_adapter == nullptr);
  m_module_adapter = &module_adapter;
  module_adapter.m_rr_adapter = this;

  if (sync) {
    // nothing to sync (this needs to notify with SC_ZERO_TIME, otherwise event
    // might be missed)
    m_synced.notify(sc_core::SC_ZERO_TIME);
  }
}

void internal::rr_target_socket_adapter::dynamic_unbind(
    module_target_socket_adapter &module_adapter) {
  // make sure we unbind from the correct socket
  sc_assert(m_module_adapter == &module_adapter);
  m_module_adapter = nullptr;
  module_adapter.m_rr_adapter = nullptr;
}

internal::module_target_socket_adapter::module_target_socket_adapter(
    tlm::tlm_target_socket<> &exposed_tsock, const sc_module_name &nm)
    : sc_core::sc_module(sc_module_name(sc_gen_unique_name(nm))),
      m_exposed_tsock(exposed_tsock) {
  // bind this interface to the internal initiator socket
  m_isock.bind(*this);
}

void internal::module_target_socket_adapter::before_end_of_elaboration() {
  // bind the internal initiator socket to the exposed target socket
  m_isock.bind(m_exposed_tsock);
}

tlm::tlm_sync_enum internal::module_target_socket_adapter::nb_transport_bw(
    tlm::tlm_base_protocol_types::tlm_payload_type &trans,
    tlm::tlm_base_protocol_types::tlm_phase_type &phase, sc_time &t) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(
      m_rr_adapter
          ->m_rr);  // TODO: should theoretically not block, so no lock required
  return m_rr_adapter->m_exposed_tsock->nb_transport_bw(trans, phase, t);
}

void internal::module_target_socket_adapter::invalidate_direct_mem_ptr(
    sc_dt::uint64 start_range, sc_dt::uint64 end_range) {
  sc_assert(m_rr_adapter != nullptr);
  reconf_lock lock(m_rr_adapter->m_rr);
  m_rr_adapter->m_exposed_tsock->invalidate_direct_mem_ptr(start_range,
                                                           end_range);
}
