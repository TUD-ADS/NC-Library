/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef REGION_RECONF_REGION_H_
#define REGION_RECONF_REGION_H_

#include "region/abstract_reconf_region_base.h"
#include "region/conf_manager.h"

/**
 * This module will represent the actual reconfiguration region that can be
 * integrated into the static design. The template argument defines the
 * interface and with this the given base class from which this reconfiguration
 * region will inherit its interface members from.
 * reconf_region provides an API to configure modules which also implement the
 * given interface (or rather the from it generated module base class). All
 * reconfiguration requests are relayed to an internal configuration manager
 * which contains the actual reconfiguration logic.
 *
 * @tparam Interface the interface class from which the base class for this
 * reconfiguration region and modules that can be configured at it are derived
 * from
 * @see conf_manager
 */
template <typename Interface,
          typename =
              std::enable_if_t<std::is_base_of<nc_interface, Interface>::value>,
          typename = std::enable_if_t<std::is_base_of<
              abstract_reconf_region_base, typename Interface::rr_base>::value>>
class reconf_region : public Interface::rr_base {
  /// @brief The base type of this class.
  typedef typename Interface::rr_base base_type;
  /// @brief Pointer type used by the configuration manager to operate on
  /// modules of this interface.
  typedef typename conf_manager<Interface>::module_ptr_t module_ptr_t;

 protected:
  /**
   * Actually load the module that was registered for preload.
   */
  void do_preload() override {
    // no adapter syncing when we perform a preload before the simulation starts
    // (non-nested preload)
    m_conf.perform_preload(this->is_nested_rr());
  }

  /**
   * Called by systemC on just when the simulation has stated.
   * Here we will trigger the preload for non-nested reconfiguration regions.
   * Nested reconfiguration regions are triggered by the module that contain
   * them on activation.
   * @see abstract_module_base#nc_activate
   */
  void start_of_simulation() override {
    if (!this->is_nested_rr()) do_preload();
  }

 public:
  /**
   * Construct a reconfiguration region with a given name.
   * Optionally, a pointer to a module of arbitrary interface can be passed,
   * which signals that this reconfiguration region is located within this
   * module. It this pointer is a nullptr, the reconfiguration region will be
   * treated as part of the static design
   *
   * @param name the name of this module
   * @param base_module a pointer to the module that contains the
   * reconfiguration region or nullptr if it is static
   */
  explicit reconf_region(const sc_core::sc_module_name& name,
                         abstract_module_base* base_module = nullptr)
      : base_type(name, base_module) {
    m_conf.connect_to_reconf_zone(this);
  }

  /**
   * Register a module of a specific type at this reconfiguration region.
   * This constructs an instance of this module and stores it for later use.
   * Every module type can only be registered once and the registration must be
   * performed during elaboration time. Only modules which where derived from
   * the interface as the reconfiguration region can be registered.
   *
   * @tparam Module the type of the module that shall be registered
   * @tparam Args types of optional arguments for the module construction
   * @param args optional arguments which may be needed to construct the module
   */
  template <class Module, typename... Args>
  void register_module(Args... args) {
    m_conf.template register_module<Module>(args...);
  }

  /**
   * Register a split manger at the reconfiguration region.
   * The registration also only can be performed during elaboration time.
   * The split manager can be configured just like a normal module during the
   * simulation time, which then models that the reconfiguration region was
   * split.
   *
   * @tparam SplitMngr the type of the split manager that shall be registered
   */
  template <class SplitMngr>
  void register_split() {
    m_conf.template register_split<SplitMngr>();
  }

  /**
   * Register a module for preloading so that it will be already load on
   * simulation start or when the module that contains the reconfiguration
   * region is activated. Only modules that have been previously registered can
   * be preload. If already one module for preloading was registered when this
   * function is called, the with this call specified module takes its place.
   *
   * @tparam Module the type of the module that shall be preload
   * @note Description copied from conf_manager#preload_module
   */
  template <class Module>
  void preload_module() {
    m_conf.template preload_module<Module>();
  }

  /**
   * Configure a module during the simulation time.
   * This will trigger the reconfiguration process which is described in more
   * detail in the configuration manager. Only modules that have been registered
   * previously can be configured. The reconfiguration process takes some time
   * and only one reconfiguration request (including unloads) can be active at a
   * time.
   *
   * @tparam Module the type of the module that shall be configured
   * @see conf_manager#configure_module
   */
  template <class Module>
  void configure_module() {
    m_conf.template configure_module<Module>();
  }

  /**
   * Unload the currently configured module.
   *
   * @implements abstract_reconf_region_base#unload_module
   */
  void unload_module() final { m_conf.unload_module(); }

  /**
   * Get a pointer to the currently configured module.
   *
   * @return instance pointer to the currently configured module or nullptr if
   * no module is configured
   * @note Description copied from conf_manager#get_current_module
   */
  auto get_current_module() const { return m_conf.get_current_module(); }

  /**
   * Get the instance pointer for the registered module type.
   *
   * @tparam Module the type for which the instance pointer shall be retrieved
   * @return the instance pointer or nullptr if the module type is not
   * registered
   * @note Description copied from conf_manager#get_module
   */
  template <class Module>
  auto get_module() const {
    return m_conf.template get_module<Module>();
  }

  /**
   * Check whether the given module type is already registered at the
   * reconfiguration region.
   * @tparam Module the type to check for
   * @return true if the module type is already registered, false otherwise
   * @note Description copied from conf_manager#is_registered
   */
  template <class Module>
  auto is_registered() const {
    return m_conf.template is_registered<Module>();
  }

 private:
  /// @brief the configuration manger that contains the reconfiguration logic
  conf_manager<Interface> m_conf;
};

#endif  // REGION_RECONF_REGION_H_
