/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef REGION_CONF_MANAGER_H_
#define REGION_CONF_MANAGER_H_

#include <sysc/kernel/sc_module.h>

#include <memory>
#include <type_traits>

#include "modules/abstract_module_base.h"
#include "modules/split_manager_base.h"
#include "nc_interface.h"

/**
 * The base class of configuration managers for any interface, which manages the
 * pointer to the reconfiguration region where the configuration manager is
 * registered and provides common functionalities on it. Since this class has no
 * template argument for the interface type, it can be specified as friend in
 * the abstract reconfiguration region base class.
 * @see abstract_reconf_region_base
 */
class conf_manager_base : public sc_core::sc_module {
  friend class split_manager_base;  // for calling unload_module_unprotected

  /**
   * Get the error message that shall be displayed when an action was invoked on
   * the configuration manager's reconfiguration region, but the configuration
   * manager has not been connected to a reconfiguration region yet.
   * @return the error message
   */
  static const char *no_base_ptr_err_msg() {
    return "Cannot perform action in conf_manager without being connected to a "
           "reconfiguration zone.";
  }

 protected:
  /**
   * Call mark_reconf_begin on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#mark_reconf_begin
   */
  void mark_reconf_begin() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    m_rr_base_ptr->mark_reconf_begin();
  }

  /**
   * Call mark_reconf_end on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#mark_reconf_end
   */
  void mark_reconf_end() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    m_rr_base_ptr->mark_reconf_end();
  }

  /**
   * Call block_transactions on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#block_transactions
   */
  void block_transactions() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    m_rr_base_ptr->block_transactions();
  }

  /**
   * Call unblock_transactions on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#unblock_transactions
   */
  void unblock_transactions() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    m_rr_base_ptr->unblock_transactions();
  }

  /**
   * Call is_reconfiguring on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#is_reconfiguring
   */
  bool is_reconfiguring() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    return m_rr_base_ptr->is_reconfiguring();
  }

  /**
   * Call transactions_blocked on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#transactions_blocked
   */
  bool transactions_blocked() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    return m_rr_base_ptr->transactions_blocked();
  }

  /**
   * Call check_unload_ok on the connected reconfiguration region.
   *
   * @see abstract_reconf_region_base#check_unload_ok
   */
  void check_unload_ok() {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    m_rr_base_ptr->is_unload_ok();
  }

  /**
   * Couple the module's members with those of the connected reconfiguration
   * region.
   *
   * @param t the module to couple
   * @param sync if true, synchronize the adapters during coupling
   */
  void couple_module(abstract_module_base *t, bool sync = true) {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    t->nc_couple(m_rr_base_ptr, sync);
  }

  /**
   * Activate the given module by spawning its processes.
   *
   * @param t the module to activate
   */
  static void activate_module(abstract_module_base *t) { t->nc_activate(); }

  /**
   * Deactivate the given module by terminating its processes.
   *
   * @param t the module to deactivate
   */
  static void deactivate_module(abstract_module_base *t) { t->nc_deactivate(); }

  /**
   * Decouple the module's members from those of the connected reconfiguration
   * region.
   *
   * @param t the module to decouple
   */
  void decouple_module(abstract_module_base *t) {
    sc_assert(m_rr_base_ptr != nullptr && no_base_ptr_err_msg());
    t->nc_decouple(m_rr_base_ptr);
  }

  /**
   * Unload the currently configured module if any, without checking whether and
   * marking that a reconfiguration is in progress. The module will only be
   * unload of it has no active transactions at the moment, otherwise this
   * function will terminate the simulation. The modules members will be
   * decoupled first, which shall always happen within one delta-cycle.
   * Afterwards, its processes are terminated (the module is deactivated).
   */
  virtual void unload_module_unprotected() = 0;

 public:
  /**
   * Create a new configuration manager base given its name.
   *
   * @param nm the name of the module
   */
  explicit conf_manager_base(const sc_core::sc_module_name &nm)
      : sc_core::sc_module(nm) {}

  /**
   * Connect this configuration manager to the given reconfiguration region.
   *
   * @param rr the reconfiguration region to connect to
   */
  void connect_to_reconf_zone(abstract_reconf_region_base *rr) {
    m_rr_base_ptr = rr;
  }

  /**
   * Disconnect this configuration manager from the given reconfiguration
   * region.
   *
   * @param rr the reconfiguration region to disconnect from
   */
  void disconnect_from_reconf_zone(abstract_reconf_region_base *rr) {
    m_rr_base_ptr = rr;
  }

 private:
  /// @brief pointer to the reconfiguration region that this configuration
  /// manager is connected too or nullptr
  abstract_reconf_region_base *m_rr_base_ptr = nullptr;
};

/**
 * This class contains the actual reconfiguration logic.
 * The general process is the same for every interface, but the concrete
 * interface members need to be known, which is why this class in templated with
 * the interface type. However, the configuration manager itself has non of
 * those members, since it does not inherit from the interface's reconfiguration
 * base class. Instead, the manager needs to be connected to a reconfiguration
 * region, so that their members can be used.
 *
 * @tparam Interface the interface type for the reconfiguration process
 */
template <class Interface,
          typename =
              std::enable_if_t<std::is_base_of<nc_interface, Interface>::value>>
class conf_manager : public conf_manager_base {
  typedef conf_manager this_type;
  typedef conf_manager_base base_type;

 public:
  typedef typename Interface::configurable_base configurable_base_type;
  typedef typename Interface::module_base module_base_type;

  typedef std::shared_ptr<configurable_base_type> module_ptr_t;

 private:
  /**
   * Try to find a module of the given type among the already registered modules
   * and return its instance pointer.
   *
   * @tparam Module the type to look for
   * @return a pointer to the modules instance or nullptr if no such module was
   * registered yet
   */
  template <class Module>
  module_ptr_t find_module() const {
    for (const auto &t : m_modules) {
      auto specific_ptr = std::dynamic_pointer_cast<Module>(t);
      if (specific_ptr != nullptr) {
        return specific_ptr;
      }
    }
    return nullptr;
  }

  /**
   * Actual implementation of the registration process after type traits have
   * been applied. Creates an instance of the module using the provided
   * arguments if any.
   *
   * @tparam Module the type of the module to create
   * @tparam Args types of optional arguments for the module construction
   * @param args optional arguments which may be needed to construct the module
   */
  template <typename Module, typename... Args>
  void do_register_module(Args... args) {
    if (sc_core::sc_is_running()) {
      SC_REPORT_ERROR(this->name(),
                      "Cannot register module during simulation time.");
    }

    if (find_module<Module>() != nullptr) {
      SC_REPORT_ERROR(this->name(), "Module already registered.");
    }
    auto t = std::make_shared<Module>(args...);
    m_modules.emplace_back(t);
  }

  /**
   * Load the as currently-active marked module.
   * This requires that no other module is load at the time, no reconfiguration
   * is ongoing and transactions are already blocked. First the members of the
   * module are coupled with those of the reconfiguration region and if the
   * passed parameter is true, the adapters are synchronized. Afterwards,
   * transactions are unblocked, and the module is activated.
   *
   * @param do_sync if true, adapters of the module and reconfiguration region
   * will synchronize
   * @note the reconfiguration-active flag is reset by this function
   */
  void load_current_module(bool do_sync = true) {
    sc_assert(
        m_current_module != nullptr &&
        "Load current module called when no module was set as current module.");
    sc_assert(is_reconfiguring() &&
              "Can only load module during ongoing reconfiguration.");
    sc_assert(transactions_blocked() &&
              "Can only load module when transactions are blocked.");

    // couple first
    couple_module(m_current_module.get(), do_sync);

    // then unblock before we activate
    unblock_transactions();

    activate_module(m_current_module.get());

    mark_reconf_end();
  }

  /**
   * Unload the currently configured module if any, without checking whether and
   * marking that a reconfiguration is in progress. The module will only be
   * unload of it has no active transactions at the moment, otherwise this
   * function will terminate the simulation. The modules members will be
   * decoupled first, which shall always happen within one delta-cycle.
   * Afterwards, its processes are terminated (the module is deactivated).
   * @note Description copied from conf_manager_base#unload_module_unprotected
   */
  void unload_module_unprotected() override {
    if (m_current_module) {
      check_unload_ok();
      decouple_module(m_current_module.get());
      deactivate_module(m_current_module.get());
      NC_REPORT_CONDITIONAL(name(),
                            "module " << m_current_module->name() << " unload")
      m_current_module = nullptr;
    }
  }

  /// @brief Default name used as base for creating a conf_manager module.
  static sc_core::sc_module_name default_name() {
    return sc_core::sc_gen_unique_name("conf_manager");
  }

 public:
  /**
   * Create a new configuration manager given its name.
   * If no name is specified, the module will receive its default name.
   *
   * @param name the name of the module or default name if none is provided
   * @see conf_manager#default_name
   */
  explicit conf_manager(sc_core::sc_module_name name = default_name())
      : conf_manager_base(name) {}

  /**
   * Wrapper function using type traits to ensure that the module has the
   * necessary members of the correct interface. Actual registration is done by
   * do_register_module.
   *
   * @tparam Module the type of the module to register
   * @tparam Args types of optional arguments for the module construction
   * @param args optional arguments which may be needed to construct the module
   * @see conf_manager#do_register_module
   */
  template <typename Module,
            typename = std::enable_if_t<
                std::is_base_of<module_base_type, Module>::value>,
            typename... Args>
  void register_module(Args... args) {
    do_register_module<Module>(args...);
  }

  /**
   * Wrapper function using type traits to ensure that the split manager has no
   * members from the interface. Actual registration procedure does not differ
   * from the one of a module and is done by do_register_module.
   *
   * @tparam SplitMngr type of the split manager
   */
  template <typename SplitMngr, typename = std::enable_if_t<std::is_base_of<
                                    configurable_base_type, SplitMngr>::value>>
  void register_split() {
    do_register_module<SplitMngr>();
  }

  /**
   * Register a module for preloading so that it will be already load on
   * simulation start or when the module that contains the reconfiguration
   * region is activated. Only modules that have been previously registered can
   * be preload. If already one module for preloading was registered when this
   * function is called, the with this call specified module takes its place.
   *
   * @tparam Module the type of the module that shall be preload
   */
  NC_INTERNAL_TMPL_BASE_OF(Module, typename Interface::module_base)
  void preload_module() {
    auto t = find_module<Module>();
    if (t == nullptr) {
      SC_REPORT_ERROR(name(), "Cannot preload module that is not registered.");
    }

    m_preload_module = t;
  }

  /**
   * Actually load a module that was registered for preloading earlier.
   * This function will mark a reconfiguration begin and block transactions, set
   * the preloaded module as current module. Afterwards, load_current_module is
   * called, which actually loads the module, unblocks transactions and marks
   * the end of the reconfiguration process. If the passed parameter is true
   * (default), the functions assumes that the module is preloaded in a nested
   * reconfiguration region adn the adapters of it will get synchronized.
   * If not module specified for preloading this function will do nothing.
   *
   * @param nested_preload if true (default), the adapters of the module will
   * get synchronized during load
   * @see conf_manager#load_current_module
   */
  void perform_preload(bool nested_preload = true) {
    // return if no preload module was registered
    if (m_preload_module == nullptr) return;

    // start like normal reconfiguration, but no wait
    mark_reconf_begin();
    block_transactions();

    m_current_module = m_preload_module;

    // only sync nested preloads
    load_current_module(nested_preload);
  }

  /**
   * Configure a module during the simulation time. Only registered modules can
   * be configured. This function should only be called if no other
   * reconfiguration request is currently in progress. If another module is
   * already load then it will be tried to unload it using
   * unload_module_unprotected. The new module is load using
   * load_current_module. Inbetween, there is a delay so that the
   * reconfiguration time can pass. This is calculated from the reconfiguration
   * speed of the interface and the modules size.
   *
   * @tparam Module the type of the module to configure
   * @see conf_manager#unload_module_unprotected,
   * conf_manager#load_current_module,
   * abstract_module_base#get_bitstream_size_bytes
   */
  NC_INTERNAL_TMPL_BASE_OF(Module, configurable_base_type)
  void configure_module() {
    auto t = find_module<Module>();
    if (t == nullptr) {
      SC_REPORT_ERROR(name(),
                      "Cannot configure module that is not registered.");
      return;
    } else if (t == m_current_module) {
      NC_REPORT_CONDITIONAL(
          name(),
          "Tried to configure module that is already configured. Ignored.");
      return;
    }

    mark_reconf_begin();
    block_transactions();

    unload_module_unprotected();

    m_current_module = t;

    // bit stream size in Byte, load speed in MByte -> * 1/1e6, convert second
    // to us -> * 1e6 => no conversion
    double load_time_us = m_current_module->get_bitstream_size_bytes() /
                          Interface::load_speed_MBps;
    NC_REPORT_CONDITIONAL(name(), "Reconfiguring module "
                                      << m_current_module->name()
                                      << ", load time [us]:" << load_time_us)
    sc_assert(load_time_us > 0 &&
              "Reconfiguration has to be larger that zero.");
    sc_core::wait(sc_core::sc_time(load_time_us, sc_core::SC_US));

    load_current_module();
  }

  /**
   * Unload the current module.
   * This function is a reconfiguration action and cannot be executed while any
   * other reconfiguration action is still in progress.
   *
   * @see conf_manager#unload_module_unprotected
   */
  void unload_module() {
    mark_reconf_begin();

    unload_module_unprotected();

    mark_reconf_end();
  }

  /**
   * Get a pointer to the instance of the currently configured module.
   *
   * @return instance pointer to the currently configured module or nullptr if
   * no module is configured
   */
  auto get_current_module() const { return m_current_module; }

  /**
   * Get the instance pointer for the registered module type.
   *
   * @tparam Module the type for which the instance pointer shall be retrieved
   * @return the instance pointer or nullptr if the module type is not
   * registered
   */
  NC_INTERNAL_TMPL_BASE_OF(Module, configurable_base_type)
  auto get_module() const {
    auto module = find_module<Module>();
    sc_assert(module != nullptr && "Cannot get module that is not register.");
    return std::dynamic_pointer_cast<Module>(module);
  }

  /**
   * Check whether the given module type is already registered at the
   * reconfiguration region.
   * @tparam Module the type to check for
   * @return true if the module type is already registered, false otherwise
   */
  NC_INTERNAL_TMPL_BASE_OF(Module, configurable_base_type)
  auto is_registered() const { return find_module<Module>() != nullptr; }

 private:
  /// @brief vector of instances of all registered modules
  std::vector<module_ptr_t> m_modules;
  /// @brief pointer to the instance of the currently configured module, may be
  /// nullptr if no module is configured
  module_ptr_t m_current_module = nullptr;
  /// @brief pointer to the instance of the module that was registered for
  /// preloading, may be nullptr
  module_ptr_t m_preload_module = nullptr;
};

#endif  // REGION_CONF_MANAGER_H_
