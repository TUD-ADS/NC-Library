#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

using namespace sc_core;

NC_INTERFACE(inout_interface) {
    sc_vector<sc_in<int>> inputs;
    sc_vector<sc_out<int>> outputs;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(inout_interface, inputs, outputs);
};



#endif /* NESTED_RECONF_NC_INTERFACE_H */
