#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

NC_INTERFACE(simple_port_interface) {
    tlm::tlm_target_socket<> target;
    tlm::tlm_initiator_socket<> initiator;

    sc_core::sc_in<int> input;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(simple_port_interface, target, initiator, input);
};

NC_INTERFACE(bidirectional_port_if) {
    tlm::tlm_target_socket<> target;
    tlm::tlm_initiator_socket<> initiator;

    sc_core::sc_in<int> input;
    sc_core::sc_out<int> output;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(bidirectional_port_if, target, initiator, input, output);
};



#endif /* NESTED_RECONF_NC_INTERFACE_H */
