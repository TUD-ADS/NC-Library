#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

NC_INTERFACE(in2out1_interface) {
    sc_core::sc_in<int> in1, in2;
    sc_core::sc_out<int> output;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(in2out1_interface, in1, in2, output);
};



#endif /* NESTED_RECONF_NC_INTERFACE_H */
