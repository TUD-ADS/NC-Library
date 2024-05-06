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
 * Model the case that we assume a module is configured and try to initiate a
 * transaction when there is actually no module configured.
 */

namespace Top3 {

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
    // reconfiguration takes 1us
    rz.configure_module<test_module>();
  }

  void do_send() {
    tlm::tlm_generic_payload payload;
    sc_time delay;
    payload.set_address(50);

    // we assume the module is configured after 500ns, which is too short
    wait(500, sc_core::SC_NS);

    isock->b_transport(payload, delay);
  }
};

struct top : public sc_module {
  reconf_region<test_interface1> rz{"rz"};
  Receiver receiver;
  Sender sender;

  top(sc_module_name nm = "top3") : sender(rz), sc_module(nm) {
    rz.register_module<test_module>();
    rz.register_module<test_module2>(2);

    receiver.tsock.bind(rz.i1);
  }
};

}  // namespace Top3

int sc_main(int argc, char **argv) {
  // should crash: "Tried to start an interaction with a module while
  // reconfiguration is in process."
  Top3::top t3;

  try {
    sc_start(4500, SC_NS);
    sc_assert(0);
  } catch (sc_report &report) {
    std::cout << "Caught report: " << report.get_msg() << std::endl;
    sc_assert(!strcmp(report.get_msg(),
                      "Tried to start an interaction with a module while "
                      "reconfiguration is in progress."));
  }

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
