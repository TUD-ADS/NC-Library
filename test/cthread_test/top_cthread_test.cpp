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

struct top : public sc_module {
  reconf_region<inout_interface> rz{"rz"};
  sc_in<int> result;
  sc_out<int> driver;
  sc_signal<int> sig1, sig2;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    result.bind(sig1);
    rz.output.bind(sig1);

    driver.bind(sig2);
    rz.input.bind(sig2);

    SC_THREAD(verify)
  }

  void verify() {
    while (true) {
      rz.configure_module<module1>();

      assert(result.read() == 0);

      for (int i = 0; i < 30; i += 3) {
        driver.write(i);
        wait(5, sc_core::SC_NS);
        assert(result.read() == i);
        wait(45, sc_core::SC_NS);
      }

      NC_REPORT_TIMED_INFO(name(), "Test of module1 was load.")

      rz.configure_module<module2>();

      assert(result.read() == 0);

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
