#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"


NC_INTERFACE(base_interface) {
    sc_core::sc_in<int> in1, in2, in3;
    sc_core::sc_out<int> output;

    // we reduce this to 100ns load time, so we can see more reconfigurations
    NC_LOAD_SPEED_MBPS(10240);

    NC_INTERFACE_REGISTER(base_interface, in1, in2, in3, output)
};

NC_INTERFACE(binary_interface) {
    sc_core::sc_in<int> a, b;
    sc_core::sc_out<int> res;

    // we reduce this to 100ns load time, so we can see more reconfigurations
    NC_LOAD_SPEED_MBPS(10240);

    NC_INTERFACE_REGISTER(binary_interface, a, b, res)
};


#endif /* NESTED_RECONF_NC_INTERFACE_H */
