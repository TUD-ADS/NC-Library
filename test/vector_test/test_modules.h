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
    inputs.init(2);
    outputs.init(2);

    NC_METHOD(drive_output1)
    sensitive << inputs[0];

    NC_METHOD(drive_output2)
    sensitive << inputs[1];

    dont_initialize();
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  void drive_output1() { outputs[0].write(2 * inputs[0].read()); }

  void drive_output2() { outputs[1].write(2 * inputs[1].read()); }
};

NC_MODULE(module2, inout_interface) {
 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, inout_interface) {
    // vectors still need to be initialized
    inputs.init(2);
    outputs.init(2);
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }
};

#endif  // NESTED_RECONF_TASKS_H
