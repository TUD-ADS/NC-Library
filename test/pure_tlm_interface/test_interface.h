#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <systemc>
#include <tlm>

#include "region/reconf_region.h"

NC_INTERFACE(test_interface1) {
    tlm::tlm_target_socket<> t1;
    tlm::tlm_initiator_socket<> i1;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(test_interface1, t1, i1);
};


#endif /* NESTED_RECONF_NC_INTERFACE_H */
