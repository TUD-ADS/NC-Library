//
// Created by alex on 16.04.23.
//

#ifndef NESTED_RECONF_RECONF_CONTROLLER_H
#define NESTED_RECONF_RECONF_CONTROLLER_H

#include <systemc>

#include "modules.h"
#include "region/reconf_region.h"

SC_MODULE(reconf_controller) {
  bool module1configured = true;
  reconf_region<controlled_interface> &rr;

  sc_core::sc_out<bool> notify_reconf;
  sc_core::sc_in<bool> ready_for_reconf;

  SC_HAS_PROCESS(reconf_controller);
  explicit reconf_controller(reconf_region<controlled_interface> & p_rr)
      : sc_module("reconf_controller"), rr(p_rr) {
    // register all modules
    rr.register_module<module1>();
    rr.register_module<module2>();

    // preload module 1 so it is configured initially
    rr.preload_module<module1>();

    SC_THREAD(loop)
  }

  [[noreturn]] void loop() {
    while (true) {
      wait(100, sc_core::SC_NS);

      if (module1configured) {
        do_reconf<module2>();
      } else {
        do_reconf<module1>();
      }
      module1configured = !module1configured;
    }
  }

  template <class T>
  void do_reconf() {
    notify_reconf.write(true);

    wait(ready_for_reconf.posedge_event());

    // wait one more NS, so we can see this event in the log. Will be reset by
    // reconfigure
    wait(1, sc_core::SC_NS);

    rr.configure_module<T>();

    notify_reconf.write(false);
  }

  int configured_module() const {
    return rr.is_reconfiguring() ? 0 : (module1configured ? 1 : 2);
  }
};

#endif  // NESTED_RECONF_RECONF_CONTROLLER_H
