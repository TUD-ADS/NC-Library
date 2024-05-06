//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

using namespace sc_core;

struct top : public sc_module {
  reconf_region<initiator_interface> rz{"rz"};
  tlm_utils::simple_target_socket<top> tsock;
  bool transaction_received = false;

  void b_transport(tlm::tlm_generic_payload& trans, sc_core::sc_time& dt) {
    sc_assert(trans.get_address() == 45);
    transaction_received = true;
  }

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    tsock.bind(rz.initiator);
    tsock.register_b_transport(this, &top::b_transport);

    SC_THREAD(do_reconf)
  }

  void do_reconf() {
    rz.configure_module<module2>();

    wait(500, sc_core::SC_NS);

    rz.configure_module<module1>();

    wait(500, sc_core::SC_NS);
  }

  bool result() const { return transaction_received; }
};

int sc_main(int argc, char** argv) {
  top t1("t1");

  sc_start(10, SC_US);

  sc_assert(t1.result());

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
