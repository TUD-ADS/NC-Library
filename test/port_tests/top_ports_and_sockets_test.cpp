//
// Created by alex on 21.02.23.
//

#ifndef NESTED_RECONF_TOP_H
#define NESTED_RECONF_TOP_H

#include <systemc>

#include "region/reconf_region.h"
#include "test_modules.h"

using namespace sc_core;

namespace Top4 {

class TlmVerifier : public sc_module {
  tlm_utils::simple_target_socket<TlmVerifier> tsock;
  tlm_utils::simple_initiator_socket<TlmVerifier> isock;
  bool configured_module1_not_module2 = false;
  int transaction_counter = 0;
  bool module1_working = false, module2a_working = false,
       module2b_working = false, loop_working = false;

  void b_transport(tlm::tlm_base_protocol_types::tlm_payload_type &trans,
                   sc_core::sc_time &t) {
    if (configured_module1_not_module2) {
      sc_assert(trans.get_address() == 42);
      module1_working = true;
    } else {
      if (transaction_counter++ % 2 == 0) {
        sc_assert(trans.get_address() == 43);
        module2a_working = true;
      } else {
        sc_assert(trans.get_address() == 44);
        module2b_working = true;

        static auto first = true;
        loop_working = !first;
        first = false;
      }
    }
  }

  sc_core::sc_event reconf_event;

 public:
  SC_HAS_PROCESS(TlmVerifier);
  explicit TlmVerifier(reconf_region<bidirectional_port_if> &rz,
                       sc_module_name nm = "tlmVerifier")
      : sc_module(sc_module_name(sc_gen_unique_name(nm))) {
    tsock.bind(rz.initiator);
    isock.bind(rz.target);
    tsock.register_b_transport(this, &TlmVerifier::b_transport);

    SC_THREAD(send_transactions)
    sensitive << reconf_event;
  }

  [[noreturn]] void send_transactions() {
    tlm::tlm_base_protocol_types::tlm_payload_type trans;
    sc_core::sc_time dt;

    while (true) {
      wait(50, sc_core::SC_NS, reconf_event);

      if (reconf_event.triggered()) {
        wait(1000, sc_core::SC_NS);
        continue;
      }

      isock->b_transport(trans, dt);
    }
  }

  void reconfiguration_imminent() { reconf_event.notify(SC_ZERO_TIME); }

  void module_reconfigured() {
    configured_module1_not_module2 = !configured_module1_not_module2;
    transaction_counter = 0;
  }

  void result() const {
    sc_assert(module1_working);
    sc_assert(module2a_working);
    sc_assert(module2b_working);
    sc_assert(loop_working);
  }
};

struct Verifier : public sc_module {
  TlmVerifier tlmVerifier;
  reconf_region<bidirectional_port_if> &rz;
  sc_out<int> out;
  sc_in<int> in;
  bool module1_working = false, module2_working = false, loop_working = false;

  SC_HAS_PROCESS(Verifier);
  explicit Verifier(reconf_region<bidirectional_port_if> &p_rz,
                    sc_module_name nm = "portVerifier")
      : sc_module(nm), rz(p_rz), tlmVerifier(p_rz) {
    SC_THREAD(verify)
  }

  void verify() {
    auto first = true;

    tlmVerifier.reconfiguration_imminent();
    NC_REPORT_TIMED_INFO(name(), "Configuring reflector_module.");
    rz.configure_module<reflector_module>();
    tlmVerifier.module_reconfigured();

    while (true) {
      for (int i = 0; i < 10; i++) {
        wait(5, SC_NS);
        out.write(i);
        wait(5, SC_NS);
        sc_assert(in.read() == i);
      }
      module1_working = true;

      tlmVerifier.reconfiguration_imminent();
      NC_REPORT_TIMED_INFO(name(), "Configuring double_reflector_module.");
      rz.configure_module<doubling_reflector_module>();
      tlmVerifier.module_reconfigured();

      for (int i = 0; i < 10; i++) {
        wait(5, SC_NS);
        out.write(i);
        wait(5, SC_NS);
        sc_assert(in.read() == 2 * i);
      }
      module2_working = true;

      tlmVerifier.reconfiguration_imminent();
      NC_REPORT_TIMED_INFO(name(), "Configuring reflector_module.");
      rz.configure_module<reflector_module>();
      tlmVerifier.module_reconfigured();

      loop_working = !first;
      first = false;
    }
  }

  void result() const {
    sc_assert(module1_working);
    sc_assert(module2_working);
    sc_assert(loop_working);
    tlmVerifier.result();
  }
};

struct top : public sc_module {
  reconf_region<bidirectional_port_if> rz{"rz"};
  Verifier verifier;
  sc_core::sc_signal<int> sig_in, sig_out;

  explicit top(sc_module_name nm) : sc_module(nm), verifier(rz) {
    rz.register_module<reflector_module>();
    rz.register_module<doubling_reflector_module>();

    verifier.out.bind(sig_in);
    rz.input.bind(sig_in);

    verifier.in.bind(sig_out);
    rz.output.bind(sig_out);
  }

  void result() const { verifier.result(); }
};

}  // namespace Top4

int sc_main(int argc, char **argv) {
  Top4::top t4("t4");
  sc_start(7000, SC_NS);

  t4.result();

  return 0;
}

#endif  // NESTED_RECONF_TOP_H
