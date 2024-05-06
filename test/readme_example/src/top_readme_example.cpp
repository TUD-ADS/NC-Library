//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

#define ARRAY_SIZE 3

SC_MODULE(top) {
  reconf_region<my_interface> rr{"rr"};
  sc_core::sc_signal<float> sig_in1, sig_in2, sig_out;
  sc_core::sc_out<float> a, b;
  sc_core::sc_in<float> result;

  float a_values[ARRAY_SIZE] = {4.1f, 2.87f, 7.3f};
  float b_values[ARRAY_SIZE] = {5.2f, 3.12f, 3.74f};

  SC_CTOR(top) {
    rr.register_module<adder>();
    rr.register_module<mul>();

    rr.in1.bind(sig_in1);
    rr.in2.bind(sig_in2);
    rr.out.bind(sig_out);

    a.bind(sig_in1);
    b.bind(sig_in2);
    result.bind(sig_out);

    SC_THREAD(run)
  }

  void run() {
    float sums[ARRAY_SIZE];
    float scalar_product = 1.f;

    rr.configure_module<adder>();

    for (int i = 0; i < ARRAY_SIZE; i++) {
      a.write(a_values[i]);
      b.write(b_values[i]);

      wait(1, sc_core::SC_NS);

      sums[i] = result.read();
    }

    rr.configure_module<mul>();

    for (int i = 0; i < ARRAY_SIZE; i++) {
      a.write(scalar_product);
      b.write(sums[i]);

      wait(1, sc_core::SC_NS);

      scalar_product = result.read();
    }

    std::cout << "Result is: " << scalar_product << std::endl;
  }
};

int sc_main(int argc, char** argv) {
  top top("top");

  sc_core::sc_start();

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
