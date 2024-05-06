/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef NC_REPORT_H_
#define NC_REPORT_H_

#include <sstream>

/// @brief Log a message from a module using SC_REPORT_INFO. The message can use
/// the shift operator of a string stream
#define NC_REPORT_TIMED_INFO(mod, msg)                                      \
  {                                                                         \
    std::stringstream ss;                                                   \
    ss << "[" << sc_core::sc_time_stamp().to_default_time_units() << "ns] " \
       << msg << std::endl;                                                 \
    SC_REPORT_INFO(mod, ss.str().c_str());                                  \
  }

/// @brief Log a message from a module using SC_REPORT_INFO. Next to the current
/// simulation time in ns the delta-cycle count for this time point is shown
/// too.
#define NC_REPORT_TIMED_DC_INFO(mod, msg)                                 \
  {                                                                       \
    std::stringstream ss;                                                 \
    ss << "[" << sc_core::sc_time_stamp().to_default_time_units()         \
       << "ns, dc: " << sc_core::sc_delta_count_at_current_time() << "] " \
       << msg << std::endl;                                               \
    SC_REPORT_INFO(mod, ss.str().c_str());                                \
  }

/// @brief Macro definition used to enable debug logging within adapters.
/// (default: disabled)
#ifdef NC_DEBUG_ADAPTER
#define NC_REPORT_DEBUG_ADAPTER(mod, msg) NC_REPORT_TIMED_DC_INFO(mod, msg)
#else
#define NC_REPORT_DEBUG_ADAPTER(mod, msg)
#endif

/// @brief Macro definition used to enable debug logging in the whole project.
/// (default: disabled)
#ifdef NC_DEBUG
#define NC_REPORT_CONDITIONAL(mod, msg) NC_REPORT_TIMED_DC_INFO(mod, msg)
#else
#define NC_REPORT_CONDITIONAL(mod, msg)
#endif

#endif  // NC_REPORT_H_
