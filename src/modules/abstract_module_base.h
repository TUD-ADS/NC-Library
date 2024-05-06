/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef TASKS_ABSTRACT_TASK_BASE_H_
#define TASKS_ABSTRACT_TASK_BASE_H_

#include <sysc/kernel/sc_module.h>

#include "processes/nc_sensitive_opts.h"
#include "processes/spawn_args.h"
#include "region/abstract_reconf_region_base.h"

class conf_manager_base;

/**
 * Abstract base for every specialized module class generated from an interface
 * description. It contains the logic for the process management and the
 * handling of nested reconfiguration regions. The module interface class,
 * generated by the macro NC_REGISTER_INTERFACE, will extend this class with the
 * necessary members.
 */
class abstract_module_base : public sc_core::sc_module {
  friend class conf_manager_base;  // for coupling and decoupling
  friend abstract_reconf_region_base::
      abstract_reconf_region_base(  // for registering contained RRs
          const sc_core::sc_module_name &name,
          abstract_module_base *base_module);

  /**
   * Register a reconfiguration region as nested within this module.
   *
   * @param region the region to register
   */
  void register_nested_rr(abstract_reconf_region_base *region);

 protected:
  /**
   * Couple the modules adapters with those of the given reconfiguration region.
   * Adapters are also synchronized if desired.
   *
   * @param rr the configuration region with which the adapters shall be coupled
   * @param sync if true, synchronize the adapters during coupling
   */
  virtual void nc_couple(abstract_reconf_region_base *rr, bool sync = true) = 0;

  /**
   * Activate this module.
   * This will spawn all of the module's processes and perform the preload on
   * every reconfiguration region that is registered as nested within this
   * module.
   */
  void nc_activate();

  /**
   * Deactivate this module.
   * This will kill all of the module's processes and perform an unload on every
   * reconfiguration region that is registered as nested within this module.
   */
  void nc_deactivate();

  /**
   * Decouple the modules adapters from those of the given reconfiguration
   * region.
   *
   * @param rr the configuration region from which the adapters shall be
   * decoupled
   */
  virtual void nc_decouple(abstract_reconf_region_base *rr) = 0;

  /**
   * Hook that is called when a module gets activated, but before any processes
   * are spawned.
   */
  virtual void on_activate() {}

  /**
   * Hook that is called when a module gets deactivated, but before any
   * processes are killed.
   */
  virtual void on_deactivate() {}

  /**
   * Register the spawn arguments of a process for this module.
   *
   * @param args the struct that describes how to spawn the process
   */
  void nc_register_spawn_args(spawn_args &&args);

 public:
  /**
   * Create an abstract module base given its name.
   *
   * @param nm the modules name
   */
  explicit abstract_module_base(const sc_core::sc_module_name &nm);

  /**
   * Get the modules size in byte.
   * Default value is 1024.
   *
   * @return the modules size
   */
  virtual size_t get_bitstream_size_bytes() const { return 1024; };

 private:
  /// @brief A vector of handles to currently running processes of the module.
  std::vector<::sc_core::sc_process_handle> m_nc_proc_handles;

  /// @brief A vector of spawn_args structs that describe how the modules
  /// processes can be created.
  std::vector<spawn_args> m_nc_proc_spawn_args;

  /// @brief A vector of pointers to reconfiguration regions that are registered
  /// as nested within this module.
  std::vector<abstract_reconf_region_base *> m_nested_regions;

 protected:
#ifndef NC_DONT_HIDE_MODULE_MEMBERS

  /**
   * Member function that shadows the "dont_initialize" method of sc_module.
   * Instead we mark that the process shall no be initialized at the currently
   * active nc_sensitive_opts.
   */
  void dont_initialize() { sensitive.dont_initialize(); }

#endif

#ifndef NC_DONT_HIDE_MODULE_MEMBERS
  /**
   * Does the same thing as the above "dont_initialize()", but without shadowing
   * the sc_module's method.
   */
  void nc_dont_initialize() { sensitive.dont_initialize(); }
#else
  /**
   * Does the same thing as the above "nc_dont_initialize()", but for the case
   * that the sc_module's method should not be shadowed.
   */
  void nc_dont_initialize() { nc_sensitive.dont_initialize(); }
#endif

#ifndef NC_DONT_HIDE_MODULE_MEMBERS
  /// @brief Used to collect information about processes sensitivities. Shadows
  /// the sc_module member "sc_sensitive".
  nc_sensitive_opts sensitive;
#else
  /// @brief Used to collect information about processes sensitivities.
  nc_sensitive_opts nc_sensitive;
#endif
};

#endif  // TASKS_ABSTRACT_TASK_BASE_H_
