/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef NC_INTERFACE_H_
#define NC_INTERFACE_H_

#include <functional>
#include <vector>

#include "forwarding/adapters.h"
#include "nc_definitions.h"

/**
 * Base class for every reconfiguration interface.
 * Does not contain any functionality or virtual functions and is purely used
 * for type-trait checks.
 */
struct nc_interface {};

#endif  // NC_INTERFACE_H_
