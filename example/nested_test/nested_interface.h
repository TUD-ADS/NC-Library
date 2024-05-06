#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"


NC_INTERFACE(io_interface) {
    sc_core::sc_in<int> input;
    sc_core::sc_out<int> output;

    // we reduce this to 100ns load time, so we can see more reconfigurations
    NC_LOAD_SPEED_MBPS(10240);

    NC_INTERFACE_REGISTER(io_interface, input, output)
};

#endif /* NESTED_RECONF_NC_INTERFACE_H */
