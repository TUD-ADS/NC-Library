/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TASKS_H
#define NESTED_RECONF_TASKS_H

#include "controlled_interface.h"

NC_MODULE(module1, controlled_interface), public tlm::tlm_bw_transport_if<> {
  bool reconf_requested = false;

 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, controlled_interface) {
    i_socket.bind(*this);

    NC_THREAD(send_transactions)

    NC_METHOD(on_reconf_request)
    sensitive << reconf_imminent.posedge_event();
    dont_initialize();
  }

  void on_activate() override { reconf_requested = false; }

  void send_transactions() {
    tlm::tlm_generic_payload p;
    sc_core::sc_time dt;
    unsigned char content = '1';
    int i = 0;

    p.set_command(tlm::TLM_WRITE_COMMAND);
    p.set_data_ptr(&content);
    p.set_data_length(1);

    while (!reconf_requested) {
      p.set_address(i++);

      i_socket->b_transport(p, dt);

      wait(5, sc_core::SC_NS);
    }

    wait(5, sc_core::SC_NS);

    // signal we are ready for reconfiguration, will be reset during
    // reconfiguration
    ready_for_reconf.write(true);
  }

  void on_reconf_request() { reconf_requested = true; }

  size_t get_bitstream_size_bytes() const override { return 100; }

  tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &,
                                     tlm::tlm_phase &, sc_core::sc_time &)
      override {
    return tlm::TLM_COMPLETED;
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64) override {}
};

NC_MODULE(module2, controlled_interface), public tlm::tlm_bw_transport_if<> {
  bool reconf_requested = false;

 public:
  SC_HAS_PROCESS(module2);
  NC_CTOR(module2, controlled_interface) {
    i_socket.bind(*this);

    NC_THREAD(send_transactions)

    NC_METHOD(on_reconf_request)
    sensitive << reconf_imminent.posedge_event();
    dont_initialize();
  }

  void on_activate() override { reconf_requested = false; }

  void send_transactions() {
    tlm::tlm_generic_payload p;
    sc_core::sc_time dt;
    unsigned char content = '2';
    int i = 50;

    p.set_command(tlm::TLM_WRITE_COMMAND);
    p.set_data_ptr(&content);
    p.set_data_length(1);

    while (!reconf_requested) {
      p.set_address(i++);

      i_socket->b_transport(p, dt);

      wait(5, sc_core::SC_NS);
    }

    wait(5, sc_core::SC_NS);

    // signal we are ready for reconfiguration
    ready_for_reconf.write(true);
  }

  void on_reconf_request() { reconf_requested = true; }

  size_t get_bitstream_size_bytes() const override { return 140; }

  tlm::tlm_sync_enum nb_transport_bw(tlm::tlm_generic_payload &,
                                     tlm::tlm_phase &, sc_core::sc_time &)
      override {
    return tlm::TLM_COMPLETED;
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64, sc_dt::uint64) override {}
};

#endif  // NESTED_RECONF_TASKS_H
