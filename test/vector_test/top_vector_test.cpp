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
  sc_vector<sc_in<int>> results;
  sc_vector<sc_out<int>> drivers;
  sc_vector<sc_signal<int>> sig_in;
  sc_vector<sc_signal<int>> sig_out;

  SC_HAS_PROCESS(top);
  explicit top(const sc_module_name& nm) : sc_module(nm) {
    rz.register_module<module1>();
    rz.register_module<module2>();

    rz.outputs.init(2);
    rz.inputs.init(2);

    results.init(2);
    drivers.init(2);
    sig_in.init(2);
    sig_out.init(2);

    results[0].bind(sig_out[0]);
    results[1].bind(sig_out[1]);
    rz.outputs[0].bind(sig_out[0]);
    rz.outputs[1].bind(sig_out[1]);

    drivers[0].bind(sig_in[0]);
    drivers[1].bind(sig_in[1]);
    rz.inputs[0].bind(sig_in[0]);
    rz.inputs[1].bind(sig_in[1]);

    SC_THREAD(verify)
  }

  void verify() {
    while (true) {
      // TODO: why does this raise Warning: (W505) object already exists:
      // t1.verify.drive_output2_0
      rz.configure_module<module1>();

      assert(results[0].read() == 0);
      assert(results[1].read() == 0);

      for (int i = 0; i < 30; i += 3) {
        drivers[0].write(i);
        drivers[1].write(i);

        wait(5, sc_core::SC_NS);

        assert(results[0].read() == 2 * i);
        assert(results[1].read() == 2 * i);

        wait(45, sc_core::SC_NS);
      }

      NC_REPORT_TIMED_INFO(name(), "Test of module1 was successful.")

      rz.configure_module<module2>();

      assert(results[0].read() == 0);
      assert(results[1].read() == 0);

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
