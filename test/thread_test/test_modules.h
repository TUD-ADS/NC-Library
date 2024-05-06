/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TASKS_H
#define NESTED_RECONF_TASKS_H

#include "bw_if_dummy.h"
#include "inout_interface.h"

NC_MODULE(module1, inout_interface) {
 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, inout_interface) {
    NC_THREAD(drive_outputs)

    dont_initialize();
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  [[noreturn]] void drive_outputs() {
    // thread should never start
    sc_assert(0);

    int i = 1;
    while (true) {
      NC_REPORT_TIMED_INFO(name(), "writing ...");
      output.write(input.read() + i++);

      wait(200, sc_core::SC_NS);
    }
  }
};

NC_MODULE(module2, inout_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, inout_interface) { NC_THREAD(drive_outputs2) }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  [[noreturn]] void drive_outputs2() {
    int i = 1;
    while (true) {
      NC_REPORT_TIMED_INFO(name(), "writing ...");
      output.write(input.read() + i);

      wait(200, sc_core::SC_NS);
    }
  }
};

#endif  // NESTED_RECONF_TASKS_H
