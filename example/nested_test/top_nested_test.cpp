//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

using namespace sc_core;

typedef reconf_region<io_interface> rz_type;

const int MAX_DEPTH = 1050;

struct Verifier : public sc_module {
  rz_type& rz;
  sc_in<int> read_rz;
  sc_out<int> write_rz;
  bool all_ok = false;

  SC_HAS_PROCESS(Verifier);
  explicit Verifier(rz_type& p_rz, const sc_module_name& nm = "verifier")
      : sc_module(nm), rz(p_rz) {
    SC_THREAD(verification)
  }

  auto& get_base_module() { return *rz.get_module<base_module>(); }

  void verification() {
    std::vector<int> adder_counts = {1, 3, 8, 20, 57, 120, 500, 1000};

    rz.configure_module<base_module>();

    for (int num_adder : adder_counts) {
      get_base_module().configure(0);

      auto starting_dc = sc_core::sc_delta_count();

      NC_REPORT_TIMED_INFO(name(), "Configuring " << num_adder << " adds")

      get_base_module().configure(num_adder);

      auto past_config_dc = sc_core::sc_delta_count();

      // reset to one, so writing zero will trigger
      write_rz.write(1);
      wait(5, SC_NS);

      write_rz.write(0);
      wait(5, SC_NS);

      NC_REPORT_TIMED_INFO(
          name(), "Expecting: " << num_adder << " Read: " << read_rz.read());
      sc_assert(read_rz.read() == num_adder);

      NC_REPORT_TIMED_INFO(name(),
                           "Took " << sc_core::sc_delta_count() - starting_dc
                                   << " dcs, only "
                                   << sc_core::sc_delta_count() - past_config_dc
                                   << " dcs without reconfiguration time.")
    }

    all_ok = true;
  }

  bool result() const { return all_ok; }
};

struct top : public sc_module {
  reconf_region<io_interface> rz{"rz"};
  Verifier verifier;
  sc_signal<int> out_signal;
  sc_signal<int> in_signal;

  explicit top(const sc_module_name& nm) : sc_module(nm), verifier(rz) {
    // only need base module here
    rz.register_module<base_module>(sc_gen_unique_name("base_module"),
                                    MAX_DEPTH);

    // bind the verifier ports to the signals exposed by the rz
    rz.input.bind(in_signal);
    verifier.write_rz.bind(in_signal);

    verifier.read_rz.bind(out_signal);
    rz.output.bind(out_signal);
  }

  bool result() const { return verifier.result(); }
};

int sc_main(int argc, char** argv) {
  top t1("t1");

  sc_start();

  sc_assert(t1.result());

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
