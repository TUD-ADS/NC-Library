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

struct Verifier : public sc_module {
  rz_type& rz;
  sc_in<int> in;
  sc_out<int> out1, out2, out3;

  SC_HAS_PROCESS(Verifier);
  explicit Verifier(rz_type& p_rz, const sc_module_name& nm = "verifier")
      : sc_module(nm), rz(p_rz) {
    SC_THREAD(verification)
  }

  int add_module_result() { return out1.read() + out2.read() + out3.read(); }

  int inner_mul_result() { return out1.read() * out2.read() + out3.read(); }

  int inner_sub_result() { return out1.read() - out2.read() + out3.read(); }

  [[noreturn]] void verification() {
    int val = 0;
    while (true) {
      NC_REPORT_TIMED_INFO(name(), "Configure mixed_module")
      rz.configure_module<mixed_module>();
      val = 0;

      // wait till first inner module is configured for 5ns
      wait(105, sc_core::SC_NS);

      NC_REPORT_TIMED_INFO(
          name(),
          "inner_mul_moduleed now configured for 5ns, verify its working ...")
      for (auto i = 0; i < 2; i++) {
        wait(95, SC_NS);

        out1.write(++val);
        out2.write(++val);
        out3.write(++val);

        wait(5, SC_NS);

        sc_assert(in.read() == inner_mul_result());
      }
      // here the inner module is configured for 205ns
      // 45ns later it will be reconfigured, which takes 100ns

      // we wait 100 ns and then try to reconfigure the top-level module
      // this should fail since it results in an unload of the nested module
      // while the nested module is reconfiguring
      wait(100, sc_core::SC_NS);
      rz.configure_module<add_module>();

      // shouldn't be reached
      sc_assert(0);
    }
  }
};

struct top : public sc_module {
  reconf_region<base_interface> rz{"rz"};
  Verifier verifier;
  sc_signal<int> out_signal;
  sc_signal<int> sig1, sig2, sig3;

  explicit top(const sc_module_name& nm) : sc_module(nm), verifier(rz) {
    rz.register_module<add_module>();
    rz.register_split<mixed_module>();

    // bind the verifier ports to the signals exposed by the rz
    rz.in1.bind(sig1);
    rz.in2.bind(sig2);
    rz.in3.bind(sig3);

    verifier.out1.bind(sig1);
    verifier.out2.bind(sig2);
    verifier.out3.bind(sig3);

    verifier.in.bind(out_signal);
    rz.output.bind(out_signal);
  }
};

}  // namespace Top1

int sc_main(int argc, char** argv) {
  Top1::top t1("t1");

  try {
    sc_start(4500, SC_NS);
    sc_assert(0);
  } catch (sc_report& report) {
    std::cout << "Caught report: " << report.get_msg() << std::endl;
    sc_assert(
        !strcmp(report.get_msg(), "Reconfiguration already in progress."));
  }

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
