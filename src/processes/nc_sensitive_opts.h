/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef PROCESSES_NC_SENSITIVE_OPTS_H_
#define PROCESSES_NC_SENSITIVE_OPTS_H_

#include <sysc/kernel/sc_spawn_options.h>
#include <systemc.h>

#include <memory>

class abstract_module_base;

/**
 * This class provides a similar interface like SystemC's sc_sensitive, but
 * instead of directly adding sensitivities to the process handle, they are
 * stored in a sc_spawn_options, which can be later retrieved to create
 * spawn_args.
 *
 * @see spawn_args
 */
class nc_sensitive_opts {
 private:
  friend class abstract_module_base;  // for instantiation

  /**
   * Only friend classes is allowed to instantiate this class.
   */
  nc_sensitive_opts() = default;

  /**
   * Note in the processes spawn options that it shall not be initialized on
   * spawn.
   */
  void dont_initialize() { m_opts->dont_initialize(); }

 public:
  /**
   * Create a new sc_spawn_options instance for the next process, given the kind
   * of the next process. Make sure to retrieve the old sc_spawn_options
   * instance beforehand since it will be destructed by this call.
   *
   * @param proc_kind the kind of the next process.
   * @return a reference to this
   * @see nc_sensitive_opts::get_opts
   */
  nc_sensitive_opts &new_args(sc_core::sc_curr_proc_kind proc_kind);

  // register event, interface, port, event_finder as sensitive (old way, with
  // function call syntax)
  nc_sensitive_opts &operator()(const sc_core::sc_event &);

  nc_sensitive_opts &operator()(sc_core::sc_interface &);

  nc_sensitive_opts &operator()(sc_core::sc_port_base &);

  nc_sensitive_opts &operator()(sc_core::sc_event_finder &);

  // register event, interface, port, event_finder as sensitive
  nc_sensitive_opts &operator<<(const sc_core::sc_event &);

  nc_sensitive_opts &operator<<(sc_core::sc_interface &);

  nc_sensitive_opts &operator<<(sc_core::sc_port_base &);

  nc_sensitive_opts &operator<<(sc_core::sc_event_finder &);

  // cthread is restricted to certain signals types (bool and logic)
  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_event_finder &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_signal_in_if<bool> &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_signal_in_if<sc_dt::sc_logic> &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_in<bool> &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_in<sc_dt::sc_logic> &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_inout<bool> &);

  nc_sensitive_opts &operator()(sc_core::sc_cthread_handle,
                                sc_core::sc_inout<sc_dt::sc_logic> &);

  /**
   * Create a new sc_spawn_options instance.
   */
  void reset();

  /**
   * Get the currently active spawn options instance.
   *
   * @return an instance of sc_spawn_options
   */
  auto get_opts() const { return m_opts; }

 private:
  /// @brief The currently active spawn options instance.
  std::shared_ptr<sc_core::sc_spawn_options> m_opts = nullptr;
};

#endif  // PROCESSES_NC_SENSITIVE_OPTS_H_
