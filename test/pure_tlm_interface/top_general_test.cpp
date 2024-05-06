//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

using namespace sc_core;

namespace Top1 {

struct Receiver {
  tlm_utils::simple_target_socket<Receiver> tsock;

  Receiver() { tsock.register_b_transport(this, &Receiver::b_transport); }

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                   sc_core::sc_time &t) {
    NC_REPORT_TIMED_INFO(
        "receiver", "Transaction to address received " << trans.get_address())
  }
};

struct Sender : public sc_module {
  tlm_utils::simple_initiator_socket<Sender> isock;
  reconf_region<test_interface1> &rz;

  SC_HAS_PROCESS(Sender);
  Sender(reconf_region<test_interface1> &p_rz, sc_module_name nm = "sender")
      : rz(p_rz), sc_module(nm) {
    isock.bind(rz.t1);

    SC_THREAD(do_send)
  }

  void do_send() {
    tlm::tlm_generic_payload payload;
    sc_time delay;

    payload.set_address(50);

    wait(50, sc_core::SC_NS);

    while (true) {
      rz.configure_module<test_module2>();

      isock->b_transport(payload, delay);

      wait(250, sc_core::SC_NS);

      rz.configure_module<test_module>();

      isock->b_transport(payload, delay);

      wait(50, sc_core::SC_NS);
    }
  }
};

struct top : public sc_module {
  reconf_region<test_interface1> rz{"rz"};
  Receiver receiver;
  Sender sender;

  top(sc_module_name nm) : sc_module(nm), sender(rz) {
    sc_assert(rz.get_current_module() == nullptr);

    std::cout << "modules can be registered and probed for" << std::endl;

    sc_assert(!rz.is_registered<test_module>());

    rz.register_module<test_module>();
    rz.register_module<test_module2>(2);

    receiver.tsock.bind(rz.i1);

    sc_assert(rz.is_registered<test_module>());
    sc_assert(rz.is_registered<test_module2>());
    sc_assert(!rz.is_registered<test_module3>());
  }
};

}  // namespace Top1

int sc_main(int argc, char **argv) {
  // should run
  Top1::top t1("t1");
  sc_start(4500, SC_NS);

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
