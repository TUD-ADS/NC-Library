/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef FORWARDING_PORT_ADAPTER_H_
#define FORWARDING_PORT_ADAPTER_H_

#include <sysc/kernel/sc_module.h>

#include "forwarding/adapter_traits.h"
#include "forwarding/nc_decoupled_value.h"
#include "nc_report.h"
#include "region/abstract_reconf_region_base.h"

namespace internal {

/**
 * Templated base class of adapters of reconfiguration regions for any SystemC
 * port type Port.
 *
 * @tparam Port the SystemC port type
 */
template <class Port>
class rr_port_adapter {};

/**
 * Templated base class of adapters of modules for any SystemC port type Port.
 *
 * @tparam Port the SystemC port type
 */
template <class Port>
class module_port_adapter {};

///
/// adapter traits specialization
///

/**
 * Specialization for sc_in<T> with generic type T.
 * Exposed type is always the sc_in<T>.
 * Adapters are rr_port_adapter<sc_in<T>> and module_port_adapter<sc_in<T>> for
 * reconfiguration region and module respectively.
 *
 * @tparam Primitive the template argument T for sc_in<T>
 * @see rr_port_adapter, module_port_adapter
 */
template <typename Primitive>
struct adapter_traits<sc_core::sc_in<Primitive>> {
  typedef sc_core::sc_in<Primitive> data_type;

  typedef sc_core::sc_in<Primitive> rr_exposed_t;
  typedef sc_core::sc_in<Primitive> module_exposed_t;

  typedef internal::rr_port_adapter<data_type> rr_adapter_t;
  typedef internal::module_port_adapter<data_type> module_adapter_t;
};

/**
 * Specialization for sc_out<T> with generic type T.
 * Exposed type is always the sc_out<T>.
 * Adapters are rr_port_adapter<sc_out<T>> and module_port_adapter<sc_out<T>>
 * for reconfiguration region and module respectively.
 *
 * @tparam Primitive the template argument T for sc_out<T>
 * @see rr_port_adapter, module_port_adapter
 */
template <typename Primitive>
struct adapter_traits<sc_core::sc_out<Primitive>> {
  typedef sc_core::sc_out<Primitive> data_type;

  typedef sc_core::sc_out<Primitive> rr_exposed_t;
  typedef sc_core::sc_out<Primitive> module_exposed_t;

  typedef internal::rr_port_adapter<data_type> rr_adapter_t;
  typedef internal::module_port_adapter<data_type> module_adapter_t;
};

///
/// sc_in<T> adapter
///

/**
 * This adapter contains a driver (sc_out) and a signal used to connect the
 * driver to the sc_in exposed to the module. When this adapter was coupled with
 * a rr_port_adapter<sc_in<T>>, the other adapter can use this driver to
 * propagate any value changes of the sc_in exposed at the static side.
 *
 * @tparam T the template argument for sc_in<T>
 * @see rr_port_adapter<sc_in<T>>
 */
template <typename T>
class module_port_adapter<sc_core::sc_in<T>> : public sc_core::sc_module {
  typedef sc_core::sc_in<T> data_type;

  friend class rr_port_adapter<data_type>;

  /// @brief Pointer to the adapter of the reconfiguration region this module is
  /// configured at. May be nullptr.
  rr_port_adapter<data_type> *m_rr_adapter = nullptr;

  /// @brief Reference to the sc_in port that is exposed by the module.
  data_type &m_exposed_port;

  /// @brief The internal sc_out port used to drive the exposed sc_in.
  sc_core::sc_out<T> m_driver;

  /// @brief The signal used to connect the internal sc_out with the exposed
  /// sc_in.
  sc_core::sc_signal<T> m_signal;

  /**
   * Get the default name for a sc_in port adapter on the module side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("module_port_adapter_sc_in");
  }

 public:
  SC_HAS_PROCESS(module_port_adapter);

  /**
   * Constructs an adapter given the adapted port that is exposed by the module
   * and optionally a name.
   *
   * @param port the sc_in port that is exposed by the module
   * @param nm (optional) the name of the module
   */
  explicit module_port_adapter(data_type &port,
                               sc_core::sc_module_name nm = default_name())
      : sc_core::sc_module(nm), m_exposed_port(port) {
    m_driver.bind(m_signal);
    m_exposed_port.bind(m_signal);
  }
};

/**
 * This adapter has a thread that is sensitive to value changes of the sc_in
 * exposed at the static side. When this adapter was coupled with a
 * module_port_adapter<sc_in<T>>, it can use the others adapter driver to
 * propagate any value changes of the sc_in exposed at the static side.
 *
 * @tparam T the template argument for sc_in<T>
 * @see module_port_adapter<sc_in<T>>
 */
template <typename T>
class rr_port_adapter<sc_core::sc_in<T>> : public sc_core::sc_module {
  typedef sc_core::sc_in<T> data_type;
  typedef sc_core::sc_in<T> exposed_type;

  friend class module_port_adapter<data_type>;

  /// @brief Pointer to the corresponding adapter of the currently configured
  /// module. May be nullptr.
  module_port_adapter<data_type> *m_module_adapter = nullptr;

  /// @brief Reference to the sc_in port that is exposed by the
  /// reconfiguration region.
  exposed_type &m_exposed_input;

  /// @brief Event used to notify the update loop that an adapter of a new
  /// module was coupled.
  sc_core::sc_event m_bind_event;

  /// @brief Flag indicating whether a synchronization is still in progress.
  bool m_syncing = false;

  /// @brief Event that is triggered when the adapter has completed
  /// synchronization.
  sc_core::sc_event m_synced;

  /**
   * Get the default name for a sc_in port adapter on the reconfiguration
   * region side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("rr_port_adapter_sc_in");
  }

 public:
  SC_HAS_PROCESS(rr_port_adapter);

  /**
   * Constructs an adapter given the reference to the reconfiguration region
   * that contains it, the adapted port that is exposed by the reconfiguration
   * region and optionally a name.
   *
   * @param rr the reconfiguration region that contains the adapter
   * @param input the sc_in port that is exposed by the module
   * @param nm (optional) the name of the module
   */
  rr_port_adapter(abstract_reconf_region_base &rr, exposed_type &input,
                  sc_core::sc_module_name nm = default_name())
      : sc_core::sc_module(nm), m_exposed_input(input) {
    SC_THREAD(update);
  }

  /**
   * This thread writes a new value to the coupled module adapter whenever the
   * value of the exposed sc_in changes or a new adapter is coupled and it
   * should be synchronized.
   */
  [[noreturn]] void update() {
    while (true) {
      wait(m_exposed_input.value_changed_event() | m_bind_event);

      if (m_module_adapter != nullptr) {
        m_module_adapter->m_driver.write(m_exposed_input.read());
        NC_REPORT_DEBUG_ADAPTER(name(),
                                "updated to value " << m_exposed_input.read());

        // notify end of sync
        if (m_syncing) {
          m_syncing = false;
          m_synced.notify(sc_core::SC_ZERO_TIME);
        }
      }
    }
  }

  /**
   * Couple the given module adapter with this adapter.
   * This sets up the forwarding procedures.
   * If the passed sync flag is true, the current value of the exposed sc_in
   * is read and written to the newly coupled adapter. When all is done, the
   * synced event will be triggered.
   *
   * @param module_adapter the module adapter that shall be coupled to this
   * adapter
   * @param sync if true, the current value of the exposed sc_in is written to
   * the coupled adapter
   */
  void dynamic_bind(module_port_adapter<data_type> &module_adapter,
                    bool sync = true) {
    // make sure nothing is bound
    sc_assert(m_module_adapter == nullptr);
    m_module_adapter = &module_adapter;
    module_adapter.m_rr_adapter = this;

    if (sync) {
      // set the current value by triggering the updater process
      m_syncing = true;
      m_bind_event.notify();
      NC_REPORT_DEBUG_ADAPTER(name(), "trigger update on bind");
    }
  }

  /**
   * Decouple the given module adapter from this adapter.
   * This tears down the forwarding procedures.
   *
   * @param module_adapter the module adapter that shall be decoupled from this
   * adapter
   */
  void dynamic_unbind(module_port_adapter<data_type> &module_adapter) {
    // make sure we unbind from the correct socket
    sc_assert(m_module_adapter == &module_adapter);
    m_module_adapter = nullptr;
    module_adapter.m_rr_adapter = nullptr;
  }

  /**
   * Retrieve the event that is triggered when synchronization completed.
   *
   * @return the synced event
   */
  auto &synced() const { return m_synced; }
};

///
/// sc_out<T> adapter
///

/**
 * This adapter contains a listener (sc_in) and a signal used to connect the
 * listener to the sc_out exposed to the module. When this adapter was coupled
 * with a rr_port_adapter<sc_out<T>>, it will use its listener to notify the
 * other adapter whenever the module wrote a new value to the exposed sc_out.
 *
 * @tparam T the template argument for sc_out<T>
 * @see rr_port_adapter<sc_out<T>>
 */
template <typename T>
class module_port_adapter<sc_core::sc_out<T>> : public sc_core::sc_module {
  typedef sc_core::sc_out<T> data_type;

  friend class rr_port_adapter<data_type>;

  /// @brief Pointer to the adapter of the reconfiguration region this module is
  /// configured at. May be nullptr.
  rr_port_adapter<data_type> *m_rr_adapter = nullptr;

  /// @brief Reference to the sc_out port that is exposed by the module.
  data_type &m_exposed_port;

  /// @brief The internal sc_in port used to listen to the exposed sc_out.
  sc_core::sc_in<T> m_listener;

  /**
   * @brief The signal used to connect the internal sc_in with the exposed
   * sc_out. SC_MANY_WRITERS policy mandatory since processes respawn on
   * reconfigure.
   */
  sc_core::sc_signal<T, sc_core::SC_MANY_WRITERS> m_signal;

  /**
   * Get the default name for a sc_out port adapter on the module side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("module_port_adapter_sc_out");
  }

 public:
  SC_HAS_PROCESS(module_port_adapter);

  /**
   * Constructs an adapter given the adapted port that is exposed by the module
   * and optionally a name.
   *
   * @param port the sc_out port that is exposed by the module
   * @param nm (optional) the name of the module
   */
  explicit module_port_adapter(data_type &port,
                               sc_core::sc_module_name nm = default_name())
      : sc_core::sc_module(nm), m_exposed_port(port) {
    m_listener.bind(m_signal);
    m_exposed_port.bind(m_signal);

    SC_THREAD(update)
  }

  /**
   * This thread notifies the currently coupled adapter that a new value needs
   * to be fetched.
   */
  [[noreturn]] void update() {
    while (true) {
      wait(m_listener.value_changed_event());

      if (m_rr_adapter != nullptr) {
        m_rr_adapter->m_update_output.notify();
        NC_REPORT_DEBUG_ADAPTER(
            name(), "triggered updated value to: " << m_listener.read());
      }
    }
  }
};

/**
 * This adapter has a driver thread that listens on an internal event, that
 * can be triggered by a coupled adapter of type module_port_adapter<sc_out<T>>.
 * Whenever the event gets triggered, the driver thread will read the value of
 * the other adapters listener and write it to the sc_out exposed at the
 * static side. Additionally, when the other adapter is decoupled again, the
 * driver thread will the decouple-value of the corresponding type to the
 * exposed sc_out.
 *
 * @tparam T the template argument for sc_out<T>
 * @see module_port_adapter<sc_out<T>>
 */
template <typename T>
class rr_port_adapter<sc_core::sc_out<T>> : public sc_core::sc_module {
  typedef sc_core::sc_out<T> data_type;
  typedef sc_core::sc_out<T> exposed_type;
  typedef nc_decoupled_value<data_type> default_value_t;

  friend class module_port_adapter<data_type>;

  /// @brief Pointer to the corresponding adapter of the currently configured
  /// module. May be nullptr.
  module_port_adapter<data_type> *m_module_adapter = nullptr;

  /// @brief Reference to the sc_out port that is exposed by the
  /// reconfiguration region.
  exposed_type &m_exposed_output;

  /// @brief Event used to notify the to notify the driver that a new value
  /// needs to be written
  sc_core::sc_event m_update_output;

  /// @brief Flag indicating whether the adapter is currently coupled with a
  /// module adapter.
  bool m_decoupled = false;

  /// @brief Event that is triggered when the adapter has completed
  /// synchronization.
  sc_core::sc_event m_synced;

  /**
   * Get the default name for a sc_out port adapter on the reconfiguration
   * region side.
   *
   * @return the default name of ths module
   */
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("rr_port_adapter_sc_out");
  }

 public:
  SC_HAS_PROCESS(rr_port_adapter);

  /**
   * Constructs an adapter given the reference to the reconfiguration region
   * that contains it, the adapted port that is exposed by the reconfiguration
   * region and optionally a name.
   *
   * @param rr the reconfiguration region that contains the adapter
   * @param output the sc_out port that is exposed by the module
   * @param nm (optional) the name of the module
   */
  rr_port_adapter(abstract_reconf_region_base &rr, exposed_type &output,
                  sc_core::sc_module_name nm = default_name())
      : sc_core::sc_module(nm), m_exposed_output(output) {
    SC_THREAD(drive_signal)
    sensitive << m_update_output;
  }

  /**
   * This thread fetches the value that was written by the module from the
   * currently coupled adapter and writes it to the sc_out exposed on the
   * static side, whenever the module writes a new value. Additionally, when the
   * other adapter gets decoupled, this thread writes the decouple-value of
   * the corresponding type to the sc_out.
   */
  [[noreturn]] void drive_signal() {
    while (true) {
      wait();

      if (m_decoupled) {
        m_exposed_output.write(default_value_t::get_value());
        continue;
      }

      // fail silently if there was a decouple in between notification and
      // triggering
      if (m_module_adapter != nullptr) {
        m_exposed_output.write(m_module_adapter->m_listener.read());
        NC_REPORT_DEBUG_ADAPTER(
            name(),
            "updated value to: " << m_module_adapter->m_listener.read());
      }
    }
  }

  /**
   * Couple the given module adapter with this adapter.
   * This sets up the forwarding procedures.
   * The adapters for sc_out do not need to by synchronized, since the module
   * was just configured and could not have written anything yet. Thus, if the
   * passed sync flag is true, then the synced event will be triggered in the
   * next delta-cycle.
   *
   * @param module_adapter the module adapter that shall be coupled to this
   * adapter
   * @param sync if true, the synced event will be triggered in the next
   * delta-cycle
   */
  void dynamic_bind(module_port_adapter<data_type> &module_adapter,
                    bool sync = true) {
    // make sure nothing is bound
    sc_assert(m_module_adapter == nullptr);
    m_module_adapter = &module_adapter;
    module_adapter.m_rr_adapter = this;

    if (sync) {
      // end decouple, allow update by module again
      m_decoupled = false;
      NC_REPORT_DEBUG_ADAPTER(name(), "decouple ended");
      m_synced.notify(sc_core::SC_ZERO_TIME);
    }
  }

  /**
   * Decouple the given module adapter from this adapter.
   * This tears down the forwarding procedures.
   * Additionally, the according decoupled flag is set and the updated event
   * triggered, so that the driver thread writes the decouple-value.
   *
   * @param module_adapter the module adapter that shall be decoupled from this
   * adapter
   * @see rr_port_adapter::m_decoupled, rr_port_adapter::m_update_output
   */
  void dynamic_unbind(module_port_adapter<data_type> &module_adapter) {
    // make sure we unbind from the correct socket
    sc_assert(m_module_adapter == &module_adapter);

    // set decouple value on both ports
    m_decoupled = true;
    m_update_output.notify();
    // we can write here since module adapter uses signal with multiple writer
    // policy
    m_module_adapter->m_exposed_port.write(default_value_t::get_value());
    NC_REPORT_DEBUG_ADAPTER(name(), "decoupled");

    m_module_adapter = nullptr;
    module_adapter.m_rr_adapter = nullptr;
  }

  /**
   * Retrieve the event that is triggered when synchronization completed.
   *
   * @return the synced event
   */
  auto &synced() const { return m_synced; }
};
}  // namespace internal

#endif  // FORWARDING_PORT_ADAPTER_H_
