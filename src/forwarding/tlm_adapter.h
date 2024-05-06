/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef FORWARDING_TLM_ADAPTER_H_
#define FORWARDING_TLM_ADAPTER_H_

#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>

#include <tlm>

#include "forwarding/adapter_traits.h"
#include "region/abstract_reconf_region_base.h"

namespace internal {

class module_initiator_socket_adapter;

class rr_initiator_socket_adapter;

class module_target_socket_adapter;

class rr_target_socket_adapter;

///
/// adapter traits specialization
///

/**
 * Specialization for tlm_initiator_socket.
 * Exposed type is always the tlm_initiator_socket.
 * Adapters are rr_initiator_socket_adapter and module_initiator_socket_adapter
 * for reconfiguration region and module respectively.
 *
 * @see rr_initiator_socket_adapter, module_initiator_socket_adapter
 */
template <>
struct adapter_traits<tlm::tlm_initiator_socket<>> {
  typedef tlm::tlm_initiator_socket<> rr_exposed_t;
  typedef tlm::tlm_initiator_socket<> module_exposed_t;

  typedef rr_initiator_socket_adapter rr_adapter_t;
  typedef module_initiator_socket_adapter module_adapter_t;
};

/**
 * Specialization for tlm_target_socket.
 * Exposed type is always the tlm_target_socket.
 * Adapters are rr_target_socket_adapter and module_target_socket_adapter for
 * reconfiguration region and module respectively.
 *
 * @see rr_target_socket_adapter, module_target_socket_adapter
 */
template <>
struct adapter_traits<tlm::tlm_target_socket<>> {
  using rr_exposed_t = tlm::tlm_target_socket<>;
  using module_exposed_t = tlm::tlm_target_socket<>;

  using rr_adapter_t = rr_target_socket_adapter;
  using module_adapter_t = module_target_socket_adapter;
};

///
/// tlm_initiator_socket adapter
///

/**
 * This adapter provides the backward path interface implementation
 * (tlm_bw_transport_if) for a tlm_initiator_socket. It thus can be bound to an
 * initiator socket that is exposed by the reconfiguration region. When it is
 * coupled with a module_initiator_socket_adapter, it can "listen" to
 * transactions relayed by the other adapter and then propagate them to the
 * static side. The forward path of the transactions works the same way.
 *
 * @see module_initiator_socket_adapter
 */
class rr_initiator_socket_adapter : public tlm::tlm_bw_transport_if<>,
                                    public sc_core::sc_module {
  friend class module_initiator_socket_adapter;

  /// @brief Reference to the reconfiguration region that this adapter is
  /// attached to.
  abstract_reconf_region_base &m_rr;

  /// @brief Pointer to the corresponding adapter of the currently configured
  /// module. May be nullptr.
  module_initiator_socket_adapter *m_module_adapter = nullptr;

  /// @brief Reference to the initiator socket that is exposed by the
  /// reconfiguration region.
  tlm::tlm_initiator_socket<> &m_exposed_isock;

  /// @brief Event that is triggered when the adapter has completed
  /// synchronization.
  sc_core::sc_event m_synced;

 public:
  /**
   * Constructs an adapter given the reference to the reconfiguration region
   * that contains it, the adapted socket that is exposed by the reconfiguration
   * region and optionally a name.
   *
   * @param rr the reconfiguration region that contains the adapter
   * @param exposed_isock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */
  explicit rr_initiator_socket_adapter(
      abstract_reconf_region_base &rr,
      tlm::tlm_initiator_socket<> &exposed_isock,
      const sc_core::sc_module_name &nm = "rr_initiator_socket_adapter");

  /**
   * Implementation of tlm_bw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * currently configured module. A reconfiguration lock is employed to ensure,
   * that the module is not unloaded while the call is still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param phase a TLM transaction phase instance
   * @param t a SystemC time
   * @return a TLM sync value
   */
  tlm::tlm_sync_enum nb_transport_bw(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans,
      tlm::tlm_base_protocol_types::tlm_phase_type &phase,
      sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_bw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * currently configured module. A reconfiguration lock is employed to ensure,
   * that the module is not unloaded while the call is still in progress.
   *
   * @param start_range a memory address
   * @param end_range a memory address
   */
  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                 sc_dt::uint64 end_range) override;

  /**
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
  void dynamic_bind(module_initiator_socket_adapter &module_adapter,
                    bool sync = true);

  /**
   * Decouple the given module adapter from this adapter.
   * This tears down the forwarding procedures.
   *
   * @param module_adapter the module adapter that shall be decoupled from this
   * adapter
   */
  void dynamic_unbind(module_initiator_socket_adapter &module_adapter);

  /**
   * Retrieve the event that is triggered when synchronization completed.
   *
   * @return the synced event
   */
  auto &synced() const { return m_synced; }
};

/**
 * This adapter contains a tlm target socket and provides the necessary
 * interface implementation (tlm_fw_transport_if) to be bound to it. This target
 * socket will in turn be bound to the tlm initiator socket that is exposed to a
 * module. When this adapter was coupled with a rr_initiator_socket_adapter, it
 * can "listen" to transactions started by the module and relay them to the
 * coupled adapter. The rr_initiator_socket_adapter will then propagate these
 * transactions to the static side. The forward path of the transactions works
 * the same way.
 *
 * @see rr_initiator_socket_adapter
 */
class module_initiator_socket_adapter : public tlm::tlm_fw_transport_if<>,
                                        public sc_core::sc_module {
  friend class rr_initiator_socket_adapter;

  /// @brief The internal target socket that is bound to the by the module
  /// exposed initiator socket.
  tlm::tlm_target_socket<> m_tsock;

  /// @brief Pointer to the adapter of the reconfiguration region this module is
  /// configured at. May be nullptr.
  rr_initiator_socket_adapter *m_rr_adapter = nullptr;

  /// @brief Reference to the initiator socket that is exposed by the module.
  tlm::tlm_initiator_socket<> &m_exposed_isock;

 public:
  /**
   * Constructs an adapter given the adapted socket that is exposed by the
   * module and optionally a name.
   *
   * @param exposed_isock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */
  explicit module_initiator_socket_adapter(
      tlm::tlm_initiator_socket<> &exposed_isock,
      const sc_core::sc_module_name &nm = "module_initiator_socket_adapter");

  /**
   * SystemC callback used to bind the internal target socket to the exposed
   * initiator socket since this has to be done after the interface
   * implementation (a module) was bound to the initiator socket.
   */
  void before_end_of_elaboration() override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param phase a TLM transaction phase instance
   * @param t a SystemC time
   * @return a TLM sync value
   */
  tlm::tlm_sync_enum nb_transport_fw(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans,
      tlm::tlm_base_protocol_types::tlm_phase_type &phase,
      sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param t a SystemC time
   */
  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                   sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param dmi_data a TLM DMI instance
   * @return boolean indication the success of the operation
   */
  bool get_direct_mem_ptr(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                          tlm::tlm_dmi &dmi_data) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @return arbitrary debug value
   */
  unsigned int transport_dbg(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans) override;
};

///
/// tlm_target_socket adapter
///

/**
 * This adapter provides the forward path interface implementation
 * (tlm_fw_transport_if) for a tlm_target_socket. It thus can be bound to an
 * target socket that is exposed by the reconfiguration region. When this
 * adapter was coupled with a module_target_socket_adapter, it can "listen" to
 * transactions started by static modules and relay them to the coupled adapter.
 * The module_target_socket_adapter will then propagate these transactions to
 * its module. The backward path of the transactions works the same way.
 *
 * @see module_target_socket_adapter
 */
class rr_target_socket_adapter : public tlm::tlm_fw_transport_if<>,
                                 public sc_core::sc_module {
  friend class module_target_socket_adapter;

  /// @brief Reference to the reconfiguration region that this adapter is
  /// attached to.
  abstract_reconf_region_base &m_rr;

  /// @brief Pointer to the corresponding adapter of the currently configured
  /// module. May be nullptr.
  module_target_socket_adapter *m_module_adapter = nullptr;

  /// @brief Reference to the target socket that is exposed by the
  /// reconfiguration region.
  tlm::tlm_target_socket<> &m_exposed_tsock;

  /// @brief Event that is triggered when the adapter has completed
  /// synchronization.
  sc_core::sc_event m_synced;

 public:
  /**
   * Constructs an adapter given the reference to the reconfiguration region
   * that contains it, the adapted socket that is exposed by the reconfiguration
   * region and optionally a name.
   *
   * @param rr the reconfiguration region that contains the adapter
   * @param exposed_isock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */
  explicit rr_target_socket_adapter(
      abstract_reconf_region_base &rr, tlm::tlm_target_socket<> &exposed_tsock,
      const sc_core::sc_module_name &nm = "rr_target_socket_adapter");

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param phase a TLM transaction phase instance
   * @param t a SystemC time
   * @return a TLM sync value
   */
  tlm::tlm_sync_enum nb_transport_fw(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans,
      tlm::tlm_base_protocol_types::tlm_phase_type &phase,
      sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param t a SystemC time
   */
  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                   sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param dmi_data a TLM DMI instance
   * @return boolean indication the success of the operation
   */
  bool get_direct_mem_ptr(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                          tlm::tlm_dmi &dmi_data) override;

  /**
   * Implementation of tlm_fw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * reconfiguration region the module is configured at. A reconfiguration lock
   * is employed to ensure, that the module is not unloaded while the call is
   * still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @return arbitrary debug value
   */
  unsigned int transport_dbg(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans) override;

  /**
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
  void dynamic_bind(module_target_socket_adapter &module_adapter,
                    bool sync = true);

  /**
   * Decouple the given module adapter from this adapter.
   * This tears down the forwarding procedures.
   *
   * @param module_adapter the module adapter that shall be decoupled from this
   * adapter
   */
  void dynamic_unbind(module_target_socket_adapter &module_adapter);

  /**
   * Retrieve the event that is triggered when synchronization completed.
   *
   * @return the synced event
   */
  auto &synced() const { return m_synced; }
};

/**
 * This adapter contains a tlm initiator socket and provides the necessary
 * interface implementation (tlm_bw_transport_if) to be bound to it. This
 * initiator socket will in turn be bound to the tlm target socket that is
 * exposed to a module. When this adapter was coupled with a
 * rr_target_socket_adapter, it can "listen" to transactions relayed by the
 * other adapter and then propagate them to the module. The backward path of the
 * transactions works the same way.
 *
 * @see rr_target_socket_adapter
 */
class module_target_socket_adapter : public tlm::tlm_bw_transport_if<>,
                                     public sc_core::sc_module {
  friend class rr_target_socket_adapter;

  /// @brief The internal initiator socket that is bound to the by the module
  /// exposed target socket.
  tlm::tlm_initiator_socket<> m_isock;

  /// @brief Reference to the target socket that is exposed by the module.
  tlm::tlm_target_socket<> &m_exposed_tsock;

  /// @brief Pointer to the adapter of the reconfiguration region this module is
  /// configured at. May be nullptr.
  rr_target_socket_adapter *m_rr_adapter = nullptr;

 public:
  /**
   * Constructs an adapter given the adapted socket that is exposed by the
   * module and optionally a name.
   *
   * @param exposed_tsock the TLM socket that is adapted
   * @param nm (optional) the name of the module
   */

  explicit module_target_socket_adapter(
      tlm::tlm_target_socket<> &exposed_tsock,
      const sc_core::sc_module_name &nm = "module_target_socket_adapter");

  /**
   * SystemC callback used to bind the internal initiator socket to the exposed
   * target socket since this has to be done after the interface implementation
   * (a module) was bound to the target socket.
   */
  void before_end_of_elaboration() override;

  /**
   * Implementation of tlm_bw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * currently configured module. A reconfiguration lock is employed to ensure,
   * that the module is not unloaded while the call is still in progress.
   *
   * @param trans a TLM transaction payload instance
   * @param phase a TLM transaction phase instance
   * @param t a SystemC time
   * @return a TLM sync value
   */
  tlm::tlm_sync_enum nb_transport_bw(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans,
      tlm::tlm_base_protocol_types::tlm_phase_type &phase,
      sc_core::sc_time &t) override;

  /**
   * Implementation of tlm_bw_transport_if<>.
   * Forwards the interface function call to the corresponding adapter of the
   * currently configured module. A reconfiguration lock is employed to ensure,
   * that the module is not unloaded while the call is still in progress.
   *
   * @param start_range a memory address
   * @param end_range a memory address
   */
  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                 sc_dt::uint64 end_range) override;
};

}  // namespace internal

#endif  // FORWARDING_TLM_ADAPTER_H_
