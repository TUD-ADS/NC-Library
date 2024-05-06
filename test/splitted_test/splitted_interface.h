#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"


NC_INTERFACE(base_interface) {
    sc_core::sc_in<int> input, input2;
    sc_core::sc_out<int> output, output2;

    // we reduce this to 100ns load time, so we can see more reconfs
    NC_LOAD_SPEED_MBPS(10240);

    NC_INTERFACE_REGISTER(base_interface, input, input2, output, output2)
};


NC_SPLIT_INTERFACE(sub_interface1, base_interface, input, output);

NC_SPLIT_INTERFACE(sub_interface2, base_interface, input2, output2);

NC_SPLIT_REGISTER(base_interface, sub_interface1, sub_interface2);

#endif /* NESTED_RECONF_NC_INTERFACE_H */
