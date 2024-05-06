#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

NC_INTERFACE(my_interface) {
    sc_core::sc_in<float> in1, in2;
    sc_core::sc_out<float> out;

    NC_LOAD_SPEED_MBPS(128);

    NC_INTERFACE_REGISTER(my_interface, in1, in2, out);
};

#endif /* NESTED_RECONF_NC_INTERFACE_H */
