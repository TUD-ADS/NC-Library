/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TASKS_H
#define NESTED_RECONF_TASKS_H

#include "bw_if_dummy.h"
#include "tlm_initiator_interface.h"

NC_MODULE(module1, initiator_interface) {
  bw_if_dummy if_dummy;

 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, initiator_interface) {
    // bind dummy interface
    initiator.bind(if_dummy);

    NC_THREAD(drive_outputs)
  }

  void on_activate() override { NC_REPORT_TIMED_INFO(name(), "was load.") }

  void drive_outputs() {
    tlm::tlm_generic_payload trans;
    sc_core::sc_time dt;

    // send transaction with address 45 the moment the module was started
    trans.set_address(45);
    initiator->b_transport(trans, dt);
  }
};

// dummy module
NC_MODULE(module2, initiator_interface) {
  bw_if_dummy if_dummy;

 public:
  NC_CTOR(module2, initiator_interface) {
    // bind dummy interface
    initiator.bind(if_dummy);
  }
};

#endif  // NESTED_RECONF_TASKS_H
