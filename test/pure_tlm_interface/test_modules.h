//
// Created by alex on 09.02.23.
//

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include <sstream>

#include "bw_if_dummy.h"
#include "test_interface.h"

NC_MODULE(test_module, test_interface1) {
  tlm_utils::simple_target_socket<test_module> simple_tsock{
      "module_simple_tsock"};

  bw_if_dummy initiator_interface_dummy;

 public:
  test_module() : test_interface1::module_base("test_module") {
    t1.bind(simple_tsock);
    i1.bind(initiator_interface_dummy);

    simple_tsock.register_b_transport(this, &test_module::b_transport);
  }

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type & trans,
                   sc_core::sc_time & t) {
    NC_REPORT_TIMED_INFO("module1", "Transaction done. Reflecting ...")

    wait(10, sc_core::SC_NS);

    i1->b_transport(trans, t);
  }

 protected:
  void on_activate() override {
    NC_REPORT_TIMED_INFO("module1", "Loading Module 1.")
  }

  void on_deactivate() override {
    NC_REPORT_TIMED_INFO("module1", "Unloading Module 1.")
  }
};

NC_MODULE(test_module2, test_interface1) {
  int i;

  bw_if_dummy initiator_interface_dummy;

  tlm_utils::simple_target_socket<test_module2> simple_tsock{
      "module2_simple_tsock"};

 public:
  SC_HAS_PROCESS(test_module2);
  explicit test_module2(int p_i)
      : i(p_i), test_interface1::module_base("test_module2") {
    t1.bind(simple_tsock);
    i1.bind(initiator_interface_dummy);

    simple_tsock.register_b_transport(this, &test_module2::b_transport);

    NC_THREAD(send_func)
  }

  void send_func() {
    tlm::tlm_generic_payload payload;
    sc_core::sc_time delay;

    while (true) {
      wait(40, sc_core::SC_NS);

      payload.set_address(i++);
      i1->b_transport(payload, delay);
    }
  }

  void on_activate() override {
    i = 0;
    NC_REPORT_TIMED_INFO("module2", "Loading Module 2.")
  }

  void on_deactivate() override {
    NC_REPORT_TIMED_INFO("module2", "Unloading Module 2.")
  }

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type & trans,
                   sc_core::sc_time & t) {
    std::stringstream ss;
    ss << "[" << sc_core::sc_time_stamp().to_default_time_units() << "ns] "
       << "Transaction done. Won't reflect. ( i=" << i << " )" << std::endl;
    SC_REPORT_INFO("module2", ss.str().c_str());
  }
};

NC_MODULE(test_module3, test_interface1){};

#endif  // NESTED_RECONF_TEST_TASKS_H
