/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include "nested_interface.h"

/// reflector module

NC_MODULE(reflector_module, io_interface) {
 public:
  SC_HAS_PROCESS(reflector_module);
  NC_CTOR(reflector_module, io_interface) {
    NC_METHOD(reflect)
    sensitive << input;
  }

  void reflect();
};

/// add one module

NC_MODULE(add_one_module, io_interface) {
 public:
  SC_HAS_PROCESS(add_one_module);
  explicit add_one_module(const sc_core::sc_module_name &nm = "add_one_module",
                          int remaining_max_depth = 1)
      : io_interface::module_base(nm), m_rz("inner_rz", this) {
    if (--remaining_max_depth > 0) {
      m_rz.register_module<add_one_module>(sc_gen_unique_name("add_one_module"),
                                           remaining_max_depth);
    }
    m_rz.register_module<reflector_module>();

    // port to control input to nested RZ
    rz_in.bind(rz_in_signal);
    m_rz.input.bind(rz_in_signal);

    // port to read output of nested RZ
    m_rz.output.bind(rz_out_signal);
    rz_out.bind(rz_out_signal);

    NC_METHOD(add_one_in)
    sensitive << input;

    dont_initialize();

    NC_METHOD(forward_out)
    sensitive << rz_out;

    dont_initialize();
  }

  void add_one_in();

  void forward_out();

  void configure(int remaining_adds);

 private:
  add_one_module &next_add();

  void finish_reflect();

  reconf_region<io_interface> m_rz;
  sc_core::sc_out<int> rz_in;
  sc_core::sc_in<int> rz_out;
  sc_core::sc_signal<int, SC_MANY_WRITERS> rz_in_signal, rz_out_signal;
};

/// base_module module

NC_MODULE(base_module, io_interface) {
 public:
  SC_HAS_PROCESS(base_module);
  explicit base_module(const sc_core::sc_module_name &nm = "base_module",
                       int max_depth = 500)
      : io_interface::module_base(nm), m_rz("inner_rz", this) {
    m_rz.register_module<add_one_module>(sc_gen_unique_name("base_module"),
                                         max_depth);
    m_rz.register_module<reflector_module>();

    // connect the rz (important to bind this way around due to hierarchical
    // binding!)
    m_rz.input.bind(input);
    m_rz.output.bind(output);
  }

  void configure(int number_of_adds);

 private:
  add_one_module &next_add();

  void finish_reflect();

  reconf_region<io_interface> m_rz;
};

#endif  // NESTED_RECONF_TEST_TASKS_H
