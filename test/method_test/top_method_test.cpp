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
  reconf_region<in2out1_interface> rz{"rz"};
  sc_in<int> result;
  sc_out<int> driver1, driver2;
  sc_signal<int> sigOut, sig1, sig2;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    result.bind(sigOut);
    rz.output.bind(sigOut);

    driver1.bind(sig1);
    rz.in1.bind(sig1);

    driver2.bind(sig2);
    rz.in2.bind(sig2);

    SC_THREAD(verify)
  }

  int expectedResult() const { return driver1.read() + driver2.read(); }

  void verify() {
    while (true) {
      rz.configure_module<module1>();

      assert(result.read() == 0);

      // set in1, will initially trigger method
      driver1.write(5);
      wait(5, sc_core::SC_NS);
      assert(result.read() == expectedResult());
      wait(45, sc_core::SC_NS);

      for (int i = 0; i < 30; i += 3) {
        // now we trigger the method with in2 multiple times
        driver2.write(i);
        wait(5, sc_core::SC_NS);
        assert(result.read() == expectedResult());
        wait(45, sc_core::SC_NS);
      }

      NC_REPORT_TIMED_INFO(name(), "Test of module1 was load.")

      // reset
      driver1.write(0);

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
