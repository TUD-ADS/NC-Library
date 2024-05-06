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
    NC_CTHREAD(drive_outputs, input)

    dont_initialize();
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  [[noreturn]] void drive_outputs() {
    while (true) {
      output.write(input.read());
      wait();
    }
  }
};

NC_MODULE(module2, inout_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, inout_interface) {
    // do nothing
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }
};

#endif  // NESTED_RECONF_TASKS_H
