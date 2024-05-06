//
// Created by alex on 16.04.23.
//

#ifndef NESTED_RECONF_STATIC_MODULE_H
#define NESTED_RECONF_STATIC_MODULE_H

#include <tlm>

SC_MODULE(static_module) {
    tlm_utils::simple_target_socket<static_module> t_socket;
    unsigned char memory[100]{};

    long transaction_to = -1;

    explicit static_module() : sc_module("static_module")
    {
        memset(memory, 0, 100);
        t_socket.register_b_transport(this, &static_module::accept_transaction);
    }

    void accept_transaction(tlm::tlm_generic_payload& p, sc_core::sc_time& dt)
    {
        transaction_to = p.get_address();

        size_t i = 0;
        for (; i < p.get_data_length(); i++) {
            memory[p.get_address()] = p.get_data_ptr()[i];
        }
        wait(i*5, sc_core::SC_NS);

        transaction_to = -1;
    }

    auto* get_mem_ptr() const { return memory; }
};


#endif //NESTED_RECONF_STATIC_MODULE_H
