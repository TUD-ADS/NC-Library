/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include "nested_interface.h"

/// inner modules

NC_MODULE(inner_mul_module, binary_interface) {
 public:
  SC_HAS_PROCESS(inner_mul_module);
  NC_CTOR(inner_mul_module, binary_interface) {
    NC_METHOD(add)
    sensitive << a << b;
  }

  void add() { res.write(a.read() * b.read()); }
};

NC_MODULE(inner_sub_module, binary_interface) {
 public:
  SC_HAS_PROCESS(inner_sub_module);
  NC_CTOR(inner_sub_module, binary_interface) {
    NC_METHOD(add)
    sensitive << a << b;
  }

  void add() { res.write(a.read() - b.read()); }
};

/// base modules

NC_MODULE(add_module, base_interface) {
 public:
  SC_HAS_PROCESS(add_module);
  NC_CTOR(add_module, base_interface) {
    NC_METHOD(calculation)
    sensitive << in1 << in2 << in3;
  }

  void calculation() { output.write(in1.read() + in2.read() + in3.read()); }
};

NC_MODULE(mixed_module, base_interface) {
 public:
  SC_HAS_PROCESS(mixed_module);
  NC_CTOR(mixed_module, base_interface)
  , m_rz("inner_rz", this) {
    m_rz.register_module<inner_mul_module>();
    m_rz.register_module<inner_sub_module>();

    // connect the rz (important to bind this way around due to hierarchical
    // binding!)
    m_rz.a.bind(in1);
    m_rz.b.bind(in2);

    m_rz.res.bind(rz_signal);
    rz_res.bind(rz_signal);

    NC_THREAD(reconf)

    NC_METHOD(calculation2)
    sensitive << in1 << rz_res;
  }

  [[noreturn]] void reconf() {
    while (true) {
      NC_REPORT_TIMED_INFO(name(), "Configuring inner_mul_module ...");
      m_rz.configure_module<inner_mul_module>();

      wait(250, sc_core::SC_NS);

      NC_REPORT_TIMED_INFO(name(), "Configuring inner_sub_module ...");
      m_rz.configure_module<inner_sub_module>();

      wait(250, sc_core::SC_NS);
    }
  }

  void calculation2() { output.write(in3.read() + m_rz.res.read()); }

 private:
  reconf_region<binary_interface> m_rz;
  sc_core::sc_signal<int> rz_signal;
  sc_core::sc_in<int> rz_res;
};

#endif  // NESTED_RECONF_TEST_TASKS_H
