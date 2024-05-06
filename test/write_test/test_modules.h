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

NC_MODULE(module1, output_interface) {
 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, output_interface) { NC_THREAD(drive_outputs) }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  [[noreturn]] void drive_outputs() {
    int i = 1;
    while (true) {
      wait(200, sc_core::SC_NS);

      NC_REPORT_TIMED_INFO(name(), "writing ...");
      output.write(i);
      output2.write(2 * i++);
    }
  }
};

NC_MODULE(module2, output_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, output_interface) { NC_THREAD(drive_outputs2) }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  [[noreturn]] void drive_outputs2() {
    int i = -1;
    while (true) {
      wait(200, sc_core::SC_NS);

      NC_REPORT_TIMED_INFO(name(), "writing ...");
      output.write(i);
      output2.write(2 * i--);
    }
  }
};

#endif  // NESTED_RECONF_TASKS_H
