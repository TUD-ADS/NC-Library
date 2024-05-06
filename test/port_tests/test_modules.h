/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include "bw_if_dummy.h"
#include "simple_port_interface.h"

NC_MODULE(module2_1, simple_port_interface) {
  tlm_utils::simple_target_socket<module2_1> simple_tsock;
  bw_if_dummy initiator_interface_dummy;

 public:
  SC_HAS_PROCESS(module2_1);
  NC_CTOR(module2_1, simple_port_interface) {
    target.bind(simple_tsock);
    initiator.bind(initiator_interface_dummy);

    NC_METHOD(transaction_on_signal)
    sensitive << input.value_changed_event();
    dont_initialize();
  }

  void on_activate() override {
    NC_REPORT_TIMED_DC_INFO(name(), "load, value of input is " << input.read())
  }

  void transaction_on_signal() {
    tlm::tlm_generic_payload payload;
    sc_core::sc_time delay;

    payload.set_address(42);

    initiator->b_transport(payload, delay);
  }
};

NC_MODULE(module2_2, simple_port_interface) {
  tlm_utils::simple_target_socket<module2_2> simple_tsock;
  bw_if_dummy initiator_interface_dummy;

 public:
  SC_HAS_PROCESS(module2_2);
  NC_CTOR(module2_2, simple_port_interface) {
    target.bind(simple_tsock);
    initiator.bind(initiator_interface_dummy);

    NC_METHOD(transaction_on_signal2)
    sensitive << input.value_changed_event();
    dont_initialize();
  }

  void on_activate() override {
    NC_REPORT_TIMED_DC_INFO(name(), "load, value of input is " << input.read())
  }

  void transaction_on_signal2() {
    tlm::tlm_generic_payload payload;
    sc_core::sc_time delay;

    payload.set_address(43);

    initiator->b_transport(payload, delay);
  }
};

NC_MODULE(reflector_module, bidirectional_port_if) {
  tlm_utils::simple_target_socket<reflector_module> simple_tsock;
  bw_if_dummy initiator_interface_dummy;
  sc_core::sc_event transaction_received;

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type & trans,
                   sc_core::sc_time & t) {
    transaction_received.notify(sc_core::SC_ZERO_TIME);
  }

 public:
  SC_HAS_PROCESS(reflector_module);
  NC_CTOR(reflector_module, bidirectional_port_if) {
    target.bind(simple_tsock);
    initiator.bind(initiator_interface_dummy);

    simple_tsock.register_b_transport(this, &reflector_module::b_transport);

    NC_METHOD(reflect_transaction)
    sensitive << transaction_received;
    dont_initialize();

    // these need to run immediately. When we couple that is not noticed as a
    // value change
    NC_METHOD(reflect_signal)
    sensitive << input.value_changed_event();
  }

  void reflect_transaction() {
    tlm::tlm_generic_payload payload;
    sc_core::sc_time delay;

    payload.set_address(42);
    initiator->b_transport(payload, delay);
  }

  void reflect_signal() { output.write(input.read()); }
};

NC_MODULE(doubling_reflector_module, bidirectional_port_if) {
  tlm_utils::simple_target_socket<doubling_reflector_module> simple_tsock;
  bw_if_dummy initiator_interface_dummy;
  sc_core::sc_event transaction_received;

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type & trans,
                   sc_core::sc_time & t) {
    transaction_received.notify(sc_core::SC_ZERO_TIME);
  }

 public:
  SC_HAS_PROCESS(doubling_reflector_module);
  NC_CTOR(doubling_reflector_module, bidirectional_port_if) {
    target.bind(simple_tsock);
    initiator.bind(initiator_interface_dummy);

    simple_tsock.register_b_transport(this,
                                      &doubling_reflector_module::b_transport);

    NC_METHOD(reflect_two_transactions)
    sensitive << transaction_received;
    dont_initialize();

    NC_METHOD(double_reflect_signal)
    sensitive << input.value_changed_event();
  }

  void reflect_two_transactions() {
    tlm::tlm_generic_payload payload;
    sc_core::sc_time delay;

    payload.set_address(43);
    initiator->b_transport(payload, delay);

    payload.set_address(44);
    initiator->b_transport(payload, delay);
  }

  void double_reflect_signal() { output.write(2 * input.read()); }
};

#endif  // NESTED_RECONF_TEST_TASKS_H
