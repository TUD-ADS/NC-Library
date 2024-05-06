/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef TASKS_SPLIT_MANAGER_BASE_H_
#define TASKS_SPLIT_MANAGER_BASE_H_

class conf_manager_base;

/**
 * Base class for every split manager created by NC_SPLIT_REGISTER.
 * Its purpose is to give the specialized split manager access to the
 * conf_manager::unload_module_unprotected.
 */
class split_manager_base {
 protected:
  /**
   * Relay the unload_module_unprotected call to the given configuration
   * manager.
   *
   * @param mngr the configuration manager on which unload_module_unprotected
   * shall be called
   */
  static void unload_module_unprotected(conf_manager_base& mngr);
};

#endif  // TASKS_SPLIT_MANAGER_BASE_H_
