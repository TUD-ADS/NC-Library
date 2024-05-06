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
  sc_in<int> in, in2;
  reconf_region<output_interface>& rz;
  int last_in1 = 0, last_in2 = 0;
  bool configured_module_1_not_2 = false, configured_module_1_not_2__ = false;

  SC_HAS_PROCESS(Listener);
  explicit Listener(reconf_region<output_interface>& p_rz,
                    sc_module_name nm = "listener")
      : rz(p_rz), sc_module(nm) {
    SC_METHOD(on_signal_1_changed)
    sensitive << in;

    dont_initialize();

    SC_METHOD(on_signal_2_changed)
    sensitive << in2;

    dont_initialize();
  }

  // expecting (0,) 1, 2, 0, -1, -2, 0, 1, 2, ...
  void on_signal_1_changed() {
    static std::stringstream ss;
    ss << in.read() << ", ";

    NC_REPORT_TIMED_INFO(name(), "in changed to: " << in.read())

    switch (last_in1) {
      case 1:
        sc_assert(in.read() == 2);
        break;
      case 2:
        sc_assert(in.read() == 0);
        break;
      case -1:
        sc_assert(in.read() == -2);
        break;
      case -2:
        sc_assert(in.read() == 0);
        break;
      case 0:
        // module configuration has changed
        configured_module_1_not_2 = !configured_module_1_not_2;

        if (configured_module_1_not_2)
          sc_assert(in.read() == 1);
        else
          sc_assert(in.read() == -1);
        break;
      default:
        sc_assert(0);
    }
    last_in1 = in;
  }

  // expecting (0,) 2, 4, 0, -2, -4, 0, 2, 4, ...
  void on_signal_2_changed() {
    static std::stringstream ss;
    ss << in2.read() << ", ";

    NC_REPORT_TIMED_INFO(name(), "in2 changed to: " << in2.read())

    switch (last_in2) {
      case 2:
        sc_assert(in2.read() == 4);
        break;
      case 4:
        sc_assert(in2.read() == 0);
        break;
      case -2:
        sc_assert(in2.read() == -4);
        break;
      case -4:
        sc_assert(in2.read() == 0);
        break;
      case 0:
        // module configuration has changed (tracked per method)
        configured_module_1_not_2__ = !configured_module_1_not_2__;

        if (configured_module_1_not_2__)
          sc_assert(in2.read() == 2);
        else
          sc_assert(in2.read() == -2);

        break;
      default:
        sc_assert(0);
    }
    last_in2 = in2;
  }
};

struct top : public sc_module {
  reconf_region<output_interface> rz{"rz"};
  Listener listener;
  sc_signal<int> listener_signal1, listener_signal2;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm), listener(rz) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    listener.in.bind(listener_signal1);
    listener.in2.bind(listener_signal2);

    rz.output.bind(listener_signal1);
    rz.output2.bind(listener_signal2);

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
