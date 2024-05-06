#ifndef NESTED_RECONF_NC_INTERFACE_H
#define NESTED_RECONF_INTERFACE_H

#include "nc_interface.h"

#include <tlm>

NC_INTERFACE(initiator_interface) {
    tlm::tlm_initiator_socket<> initiator;

    NC_LOAD_SPEED_MBPS(1024);

    NC_INTERFACE_REGISTER(initiator_interface, initiator);
};



#endif /* NESTED_RECONF_NC_INTERFACE_H */
