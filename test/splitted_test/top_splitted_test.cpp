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

typedef reconf_region<base_interface> rz_type;

struct Driver : public sc_module {
  reconf_region<base_interface>& rz;
  sc_out<int> out1, out2;

  SC_HAS_PROCESS(Driver);
  explicit Driver(rz_type& p_rz, const sc_module_name& nm = "driver")
      : sc_module(nm), rz(p_rz) {
    SC_THREAD(drive_signals)
    SC_THREAD(reconfigure)
  }

  void reconfigure() {
    NC_REPORT_TIMED_INFO(name(), "Configure module1")
    rz.configure_module<module1>();

    wait(500, SC_NS);

    NC_REPORT_TIMED_INFO(name(), "Configure sub_interface1: module2_1a")
    NC_REPORT_TIMED_INFO(name(), "Configure sub_interface2: module2_2")

    rz.configure_module<base_interface_manager>();
    auto hub = rz.get_module<base_interface_manager>();
    hub->configure_module_sub_interface1<module2_1a>();
    hub->configure_module_sub_interface2<module2_2>();

    wait(500, SC_NS);

    NC_REPORT_TIMED_INFO(name(), "Configure sub_interface1: module2_1b")

    hub->configure_module_sub_interface1<module2_1b>();

    wait(500, SC_NS);

    NC_REPORT_TIMED_INFO(name(), "Configure module1")

    rz.configure_module<module1>();
  }

  [[noreturn]] void drive_signals() {
    int i = 0;
    while (true) {
      wait(200, SC_NS);

      out1.write(++i);
      out2.write(i);
    }
  }
};

struct Listener : public sc_module {
  sc_in<int> in1, in2;

  SC_HAS_PROCESS(Listener);
  explicit Listener(const sc_module_name& nm = "listener") : sc_module(nm) {
    SC_METHOD(listen_in1)
    sensitive << in1;

    SC_METHOD(listen_in2)
    sensitive << in2;

    dont_initialize();
  }

  void listen_in1() {
    NC_REPORT_TIMED_INFO(name(), "in1 changed to " << in1.read())
  }

  void listen_in2() {
    NC_REPORT_TIMED_INFO(name(), "in2 changed to " << in2.read())
  }
};

struct top : public sc_module {
  reconf_region<base_interface> rz{"rz"};
  Driver driver;
  Listener listener;
  sc_signal<int> driver_signal1, driver_signal2, listener_signal1,
      listener_signal2;

  explicit top(const sc_module_name& nm) : sc_module(nm), driver(rz) {
    // register a standard module and the hub required for nesting
    rz.register_module<module1>();
    rz.register_split<base_interface_manager>();

    // register modules for the nested interfaces
    auto hub = rz.get_module<base_interface_manager>();
    hub->register_module_sub_interface1<module2_1a>();
    hub->register_module_sub_interface1<module2_1b>();

    hub->register_module_sub_interface2<module2_2>();

    // bind the driver and listener ports to the signals exposed by the rz
    driver.out1.bind(driver_signal1);
    driver.out2.bind(driver_signal2);

    rz.input.bind(driver_signal1);
    rz.input2.bind(driver_signal2);

    listener.in1.bind(listener_signal1);
    listener.in2.bind(listener_signal2);

    rz.output.bind(listener_signal1);
    rz.output2.bind(listener_signal2);
  }
};

}  // namespace Top1

int sc_main(int argc, char** argv) {
  Top1::top t1("t1");

  sc_start(4500, SC_NS);

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
