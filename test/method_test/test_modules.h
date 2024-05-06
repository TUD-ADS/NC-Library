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

NC_MODULE(module1, in2out1_interface) {
 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, in2out1_interface) {
    NC_METHOD(drive_outputs);
    sensitive << in1;

    dont_initialize();
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  void drive_outputs() {
    output.write(in1.read() + in2.read());

    // will only the first time be triggered by in1
    next_trigger(in2.value_changed_event());
  }
};

NC_MODULE(module2, in2out1_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, in2out1_interface) {
    // do nothing
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }
};

#endif  // NESTED_RECONF_TASKS_H
