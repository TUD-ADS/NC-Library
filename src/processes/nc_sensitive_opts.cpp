/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include "processes/nc_sensitive_opts.h"

nc_sensitive_opts &nc_sensitive_opts::new_args(
    sc_core::sc_curr_proc_kind proc_kind) {
  reset();
  if (proc_kind == sc_core::SC_METHOD_PROC_) m_opts->spawn_method();
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    const sc_core::sc_event &event) {
  m_opts->set_sensitivity(&event);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_interface &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(sc_core::sc_port_base &port) {
  m_opts->set_sensitivity(&port);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_event_finder &finder) {
  m_opts->set_sensitivity(&finder);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator<<(const sc_event &event) {
  operator()(event);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator<<(sc_interface &interface) {
  operator()(interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator<<(sc_port_base &port) {
  operator()(port);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator<<(sc_event_finder &finder) {
  operator()(finder);
  return *this;
}

void nc_sensitive_opts::reset() {
  m_opts = std::make_shared<sc_spawn_options>();
}

nc_sensitive_opts &nc_sensitive_opts::operator()(sc_core::sc_cthread_handle,
                                                 sc_event_finder &finder) {
  operator()(finder);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_cthread_handle, sc_signal_in_if<bool> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_cthread_handle, sc_signal_in_if<sc_dt::sc_logic> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(sc_core::sc_cthread_handle,
                                                 sc_in<bool> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_cthread_handle, sc_in<sc_dt::sc_logic> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(sc_core::sc_cthread_handle,
                                                 sc_inout<bool> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}

nc_sensitive_opts &nc_sensitive_opts::operator()(
    sc_core::sc_cthread_handle, sc_inout<sc_dt::sc_logic> &interface) {
  m_opts->set_sensitivity(&interface);
  return *this;
}
