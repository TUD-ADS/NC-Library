/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TASKS_H
#define NESTED_RECONF_TASKS_H

#include "bw_if_dummy.h"
#include "simple_port_interface.h"

using namespace sc_core;

/**
 * Test whether modules can have signals with single_writer_policy
 */

NC_MODULE(module1, io_interface) {
  sc_in<int> in;
  sc_out<int> out;
  sc_signal<int> sig;

 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, io_interface) {
    // bind signal
    in.bind(sig);
    out.bind(sig);

    // this will be the driver of the internal signal
    NC_METHOD(listen_input)
    sensitive << input;
    nc_dont_initialize();

    // this will react to it
    NC_THREAD(drive_outputs)
    sensitive << in;
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  void listen_input() { out.write(input.read()); }

  [[noreturn]] void drive_outputs() {
    while (true) {
      wait();

      NC_REPORT_TIMED_INFO(name(), "writing ...");
      output.write(in.read());
    }
  }
};

NC_MODULE(module2, io_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, io_interface) {
    NC_METHOD(drive_outputs2)
    sensitive << input;
    dont_initialize();
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  void drive_outputs2() { output.write(2 * input.read()); }
};

#endif  // NESTED_RECONF_TASKS_H
