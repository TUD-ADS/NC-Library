/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef PROCESSES_SPAWN_ARGS_H_
#define PROCESSES_SPAWN_ARGS_H_

#include <sysc/kernel/sc_spawn_options.h>

#include <functional>
#include <memory>

/**
 * Triplet that describes how a process can be spawned.
 * It contains the function that shall be executed by the process, the processes
 * names and a sc_spawn_options instance which gives information about the
 * processes type, its sensitivities, whether it shall be initialized and so on.
 */
struct spawn_args {
  /// @brief The function that shall be executed by the process.
  std::function<void()> func;
  /// @brief The name of the process.
  const char* name;
  /// @brief A pointer to the sc_spawn_options describing the accurate behavior
  /// of the process.
  std::shared_ptr<sc_core::sc_spawn_options> opts;
};

#endif  // PROCESSES_SPAWN_ARGS_H_
