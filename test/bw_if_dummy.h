/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_BW_IF_DUMMY_H
#define NESTED_RECONF_BW_IF_DUMMY_H

#include "tlm_core/tlm_2/tlm_2_interfaces/tlm_fw_bw_ifs.h"

struct bw_if_dummy : public tlm::tlm_bw_transport_if<> {
  tlm::tlm_sync_enum nb_transport_bw(
      tlm::tlm_base_protocol_types::tlm_payload_type &trans,
      tlm::tlm_base_protocol_types::tlm_phase_type &phase,
      sc_core::sc_time &t) override {
    return tlm::TLM_COMPLETED;
  }

  void invalidate_direct_mem_ptr(sc_dt::uint64 start_range,
                                 sc_dt::uint64 end_range) override {}
};

#endif  // NESTED_RECONF_BW_IF_DUMMY_H
