//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#define NC_DEBUG

#include <fstream>
#include <systemc>

#include "reconf_controller.h"
#include "static_module.h"

using namespace sc_core;

namespace Top1 {

struct top : public sc_module {
  reconf_region<controlled_interface> rr{"rz"};
  sc_signal<bool> reconf_sig, ready_sig;

  reconf_controller rc;
  static_module sm;

  SC_HAS_PROCESS(top);

  explicit top(const sc_module_name &nm) : sc_module(nm), rc(rr) {
    // connect the signals
    rr.reconf_imminent.bind(reconf_sig);
    rc.notify_reconf.bind(reconf_sig);

    rr.ready_for_reconf.bind(ready_sig);
    rc.ready_for_reconf.bind(ready_sig);

    rr.i_socket(sm.t_socket);

    SC_THREAD(log);
  }

  void log() {
    std::ofstream csv;
    csv.open("log.csv");
    csv << "time,rm,reconf,ready,addr" << std::endl;

    while (true) {
      csv << sc_time_stamp().to_default_time_units() << ","
          << rc.configured_module() << "," << reconf_sig.read() << ","
          << ready_sig.read() << "," << sm.transaction_to << std::endl;

      wait(1, sc_core::SC_NS);
    }
  }
};

}  // namespace Top1

int sc_main(int argc, char **argv) {
  Top1::top t1("t1");

  sc_start(2.5, SC_US);

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
