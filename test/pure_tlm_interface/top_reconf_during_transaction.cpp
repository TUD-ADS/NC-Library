//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

using namespace sc_core;

/**
 * Model the case that there is an active transaction (e.g. b_transport called
 * and wait called in module) when we want to configure a different module.
 */

namespace Top2 {

struct Receiver {
  tlm_utils::simple_target_socket<Receiver> tsock;

  Receiver() { tsock.register_b_transport(this, &Receiver::b_transport); }

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                   sc_core::sc_time &t) {
    NC_REPORT_TIMED_INFO("receiver", "Transaction received")
  }
};

struct Sender : public sc_module {
  tlm_utils::simple_initiator_socket<Sender> isock;
  reconf_region<test_interface1> &rz;

  SC_HAS_PROCESS(Sender);
  Sender(reconf_region<test_interface1> &p_rz, sc_module_name nm = "sender")
      : rz(p_rz), sc_module(nm) {
    isock.bind(rz.t1);

    SC_THREAD(do_reconf)
    SC_THREAD(do_send)
  }

  void do_reconf() {
    // b_transport is called at 1us + 50ns and will block for 10ns
    // -> try to reconfigure at 1055ns (during of b_transport)
    wait(1055, sc_core::SC_NS);

    rz.configure_module<test_module2>();
  }

  void do_send() {
    tlm::tlm_generic_payload payload;
    sc_time delay;
    payload.set_address(50);

    // takes 1us
    rz.configure_module<test_module>();

    wait(50, sc_core::SC_NS);

    isock->b_transport(payload, delay);
  }
};

struct top : public sc_module {
  reconf_region<test_interface1> rz{"rz"};
  Receiver receiver;
  Sender sender;

  top(sc_module_name nm = "top2") : sender(rz), sc_module(nm) {
    rz.register_module<test_module>();
    rz.register_module<test_module2>(2);

    receiver.tsock.bind(rz.i1);
  }
};

}  // namespace Top2

int sc_main(int argc, char **argv) {
  // should crash: "Cannot reconfigure a module while there are still active
  // interaction with it."
  Top2::top t2;

  try {
    sc_start(4500, SC_NS);
    sc_assert(0);
  } catch (sc_report &report) {
    std::cout << "Caught report: " << report.get_msg() << std::endl;
    sc_assert(!strcmp(report.get_msg(),
                      "Cannot reconfigure a module while there are still "
                      "active interaction with it."));
  }

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
