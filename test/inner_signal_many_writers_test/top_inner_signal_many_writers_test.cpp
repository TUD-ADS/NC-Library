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

struct Verifier : sc_module {
  sc_out<int> out;
  sc_in<int> in;
  reconf_region<io_interface>& rz;
  int written_in = 0;
  bool configured_module_1_not_2 = true, decoupled = false;

  SC_HAS_PROCESS(Verifier);
  explicit Verifier(reconf_region<io_interface>& p_rz,
                    sc_module_name nm = "listener")
      : rz(p_rz), sc_module(nm) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    // rz.preload_module<module1>();

    SC_THREAD(verify);

    SC_METHOD(on_signal_1_changed)
    sensitive << in;
    dont_initialize();
  }

  void verify() {
    rz.configure_module<module1>();
    while (true) {
      wait(50, sc_core::SC_NS);
      out.write(++written_in);
      wait(50, sc_core::SC_NS);

      decoupled = true;
      rz.configure_module<module2>();
      decoupled = false;
      configured_module_1_not_2 = !configured_module_1_not_2;

      wait(50, sc_core::SC_NS);
      out.write(++written_in);
      wait(50, sc_core::SC_NS);

      decoupled = true;
      rz.configure_module<module1>();
      decoupled = false;
      configured_module_1_not_2 = !configured_module_1_not_2;
    }
  }

  // expecting (0,) 1, 2, 0, -1, -2, 0, 1, 2, ...
  void on_signal_1_changed() {
    if (decoupled) {
      sc_assert(in.read() == 0);
      return;
    }

    if (configured_module_1_not_2) {
      sc_assert(written_in == in.read());
    } else {
      sc_assert(2 * written_in == in.read());
    }
  }
};

struct top : public sc_module {
  reconf_region<io_interface> rz{"rz"};
  Verifier verifier;
  sc_signal<int> listener_signal, driver_signal;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm), verifier(rz) {
    verifier.in.bind(listener_signal);
    verifier.out.bind(driver_signal);

    rz.output.bind(listener_signal);
    rz.input.bind(driver_signal);
  }
};

}  // namespace Top1

int sc_main(int argc, char** argv) {
  Top1::top t1("t1");

  sc_start(10, SC_US);

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
