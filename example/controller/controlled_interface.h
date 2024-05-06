#ifndef NESTED_RECONF_CONTROLLER_INTERFACE_H
#define NESTED_RECONF_CONTROLLER_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

NC_INTERFACE(controlled_interface) {
    sc_core::sc_in<bool> reconf_imminent;
    sc_core::sc_out<bool> ready_for_reconf;

    tlm::tlm_initiator_socket<> i_socket;

    NC_LOAD_SPEED_MBPS(380); // ICAP
    //NC_LOAD_SPEED_MBPS(128); // PCAP

    NC_INTERFACE_REGISTER(controlled_interface, reconf_imminent, ready_for_reconf, i_socket);
};

#endif /* NESTED_RECONF_CONTROLLER_INTERFACE_H */
