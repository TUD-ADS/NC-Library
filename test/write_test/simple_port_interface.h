#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

NC_INTERFACE(output_interface) {
    sc_core::sc_out<int> output, output2;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(output_interface, output, output2);
};



#endif /* NESTED_RECONF_NC_INTERFACE_H */
