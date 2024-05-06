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

struct Listener : sc_module {
  sc_in<int> in;
  reconf_region<inout_interface>& rz;
  int last_in1 = 0, last_in2 = 0;

  SC_HAS_PROCESS(Listener);
  explicit Listener(reconf_region<inout_interface>& p_rz,
                    sc_module_name nm = "listener")
      : rz(p_rz), sc_module(nm) {
    SC_METHOD(on_signal_changed)
    sensitive << in;

    dont_initialize();
  }

  void on_signal_changed() {
    NC_REPORT_TIMED_INFO(name(), "in changed to: " << in.read())
  }
};

struct top : public sc_module {
  reconf_region<inout_interface> rz{"rz"};
  Listener listener;
  sc_out<int> driver;
  sc_signal<int> sig1, sig2;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm), listener(rz) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    listener.in.bind(sig1);
    rz.output.bind(sig1);

    driver.bind(sig2);
    rz.input.bind(sig2);

    SC_THREAD(do_reconf)
  }

  void do_reconf() {
    while (true) {
      rz.configure_module<module1>();

      wait(500, sc_core::SC_NS);

      rz.configure_module<module2>();

      wait(500, sc_core::SC_NS);
    }
  }
};

}  // namespace Top1

int sc_main(int argc, char** argv) {
  Top1::top t1("t1");

  sc_start(10, SC_US);

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
