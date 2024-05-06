/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef NC_DEFINITIONS_H_
#define NC_DEFINITIONS_H_

#include "modules/abstract_module_base.h"
#include "nc_report.h"

/// @brief Create a reconfiguration interface given its name.
#define NC_INTERFACE(name) struct name : public nc_interface

/// @brief Define the reconfiguration speed for the given interface in MeBi per
/// second.
#define NC_LOAD_SPEED_MBPS(speed) \
  constexpr static double load_speed_MBps = speed
/// @brief Create a module given its name and the reconfiguration interface it
/// is based on.
#define NC_MODULE(name, interface) struct name : public interface::module_base
/// @brief Create the constructor for a module with the given name and its
/// interface. Initializes the interface base class accordingly.
#define NC_CTOR(name, interface)                        \
  explicit name(const sc_core::sc_module_name& nm =     \
                    sc_core::sc_gen_unique_name(#name)) \
      : interface::module_base(nm)

/// @brief Define a template parameter, but only enable it if the type is
/// derived from the given base type.
#define NC_INTERNAL_TMPL_BASE_OF(targ, base) \
  template <typename targ,                   \
            typename = std::enable_if_t<std::is_base_of<base, targ>::value>>

/// @brief Macro pair to count the number of arguments, up to 10.
#define NC_INTERNAL_TWELFTH_ARG(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, \
                                a12, ...)                                     \
  a12
#define NC_INTERNAL_COUNT(...)                                                 \
  NC_INTERNAL_TWELFTH_ARG(dummy, ##__VA_ARGS__, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, \
                          0)

/// @brief Macros to concatenate to values in one or two iterations
/// respectively.
#define NC_INTERNAL_CONCAT_(a, b) a##b
#define NC_INTERNAL_CONCAT(a, b) NC_INTERNAL_CONCAT_(a, b)

/// @brief helper macros for NC_INTERNAL_FOREACH
#define NC_INTERNAL_CALL_1(mac, var) mac(var);
#define NC_INTERNAL_CALL_2(mac, var, var2) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_1(mac, var2)
#define NC_INTERNAL_CALL_3(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_2(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_4(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_3(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_5(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_4(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_6(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_5(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_7(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_6(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_8(mac, var, ...) \
  NC_INTERNAL_CALL_1(mac, var) NC_INTERNAL_CALL_7(mac, __VA_ARGS__)

/// @brief Interprets the first argument as callable-macro and call it with each
/// of the passed variables, each call is followed by a semicolon.
#define NC_INTERNAL_FOREACH(mac, ...)                                   \
  NC_INTERNAL_CONCAT(NC_INTERNAL_CALL_, NC_INTERNAL_COUNT(__VA_ARGS__)) \
  (mac, __VA_ARGS__)

/// @brief helper macros for NC_INTERNAL_FOREACH_LIST
#define NC_INTERNAL_CALL_LIST_1(mac, var) mac(var)
#define NC_INTERNAL_CALL_LIST_2(mac, var, var2) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_1(mac, var2)
#define NC_INTERNAL_CALL_LIST_3(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_2(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_LIST_4(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_3(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_LIST_5(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_4(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_LIST_6(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_5(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_LIST_7(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_6(mac, __VA_ARGS__)
#define NC_INTERNAL_CALL_LIST_8(mac, var, ...) \
  NC_INTERNAL_CALL_LIST_1(mac, var), NC_INTERNAL_CALL_LIST_7(mac, __VA_ARGS__)

/// @brief Interprets the first argument as callable-macro and call it with each
/// of the passed variables, seperated by commas.
#define NC_INTERNAL_FOREACH_LIST(mac, ...)                                   \
  NC_INTERNAL_CONCAT(NC_INTERNAL_CALL_LIST_, NC_INTERNAL_COUNT(__VA_ARGS__)) \
  (mac, __VA_ARGS__)

/// @brief helper macros for NC_INTERNAL_FOREACH_1ARG
#define NC_INTERNAL_CALL_1ARG_1(mac, arg, var) mac(arg, var);
#define NC_INTERNAL_CALL_1ARG_2(mac, arg, var, var2) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var) NC_INTERNAL_CALL_1ARG_1(mac, arg, var2)
#define NC_INTERNAL_CALL_1ARG_3(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_2(mac, arg, __VA_ARGS__)
#define NC_INTERNAL_CALL_1ARG_4(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_3(mac, arg, __VA_ARGS__)
#define NC_INTERNAL_CALL_1ARG_5(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_4(mac, arg, __VA_ARGS__)
#define NC_INTERNAL_CALL_1ARG_6(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_5(mac, arg, __VA_ARGS__)
#define NC_INTERNAL_CALL_1ARG_7(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_6(mac, arg, __VA_ARGS__)
#define NC_INTERNAL_CALL_1ARG_8(mac, arg, var, ...) \
  NC_INTERNAL_CALL_1ARG_1(mac, arg, var)            \
  NC_INTERNAL_CALL_1ARG_7(mac, arg, __VA_ARGS__)

/// @brief Interprets the first argument as callable-macro with two arguments.
/// Call it with each of the passed variables, but keep the first variable
/// always the same. Seperated by commas.
#define NC_INTERNAL_FOREACH_1ARG(mac, arg, ...)                              \
  NC_INTERNAL_CONCAT(NC_INTERNAL_CALL_1ARG_, NC_INTERNAL_COUNT(__VA_ARGS__)) \
  (mac, arg, __VA_ARGS__)

/// @brief Macro pair that defines the members and their according adapters for
/// a reconfiguration region of the given interface.
#define NC_INTERNAL_RR_MEMBER(iname, var)                           \
 public:                                                            \
  internal::adapter_traits<decltype(iname::var)>::rr_exposed_t var; \
                                                                    \
 private:                                                           \
  internal::adapter_traits<decltype(iname::var)>::rr_adapter_t var##_adapter
#define NC_INTERNAL_RR_MEMBER_LIST(iname, ...) \
  NC_INTERNAL_FOREACH_1ARG(NC_INTERNAL_RR_MEMBER, iname, __VA_ARGS__)

/// @brief Macro pair that defines a reference of the given adapters.
#define NC_INTERNAL_RR_MEMBER_REF(var) \
  decltype(var##_adapter)& var##_adapter_ref
#define NC_INTERNAL_RR_MEMBER_REFS(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_RR_MEMBER_REF, __VA_ARGS__)

/// @brief Macro pair that initializes each of the adapter references defined by
/// NC_INTERNAL_RR_MEMBER_REFS with the according adapter.
#define NC_INTERNAL_RR_MEMBER_REF_INIT(var) var##_adapter_ref(rr.var##_adapter)
#define NC_INTERNAL_RR_MEMBER_REFS_INIT(...) \
  NC_INTERNAL_FOREACH_LIST(NC_INTERNAL_RR_MEMBER_REF_INIT, __VA_ARGS__)

/// @brief Macro pair that initializes every given adapter with a reference to
/// the this (reconfiguration region) and the adapted member.
#define NC_INTERNAL_INIT_RR_MEMBER(var) var##_adapter(*this, var)
#define NC_INTERNAL_INIT_RR_MEMBERS(...) \
  NC_INTERNAL_FOREACH_LIST(NC_INTERNAL_INIT_RR_MEMBER, __VA_ARGS__)

/// @brief Macro pair that defines the members and their according adapters for
/// module of the given interface.
#define NC_INTERNAL_TASK_MEMBER(iname, var)                             \
 public:                                                                \
  internal::adapter_traits<decltype(iname::var)>::module_exposed_t var; \
                                                                        \
 private:                                                               \
  internal::adapter_traits<decltype(iname::var)>::module_adapter_t var##_adapter
#define NC_INTERNAL_TASK_MEMBER_LIST(iname, ...) \
  NC_INTERNAL_FOREACH_1ARG(NC_INTERNAL_TASK_MEMBER, iname, __VA_ARGS__)

/// @brief Macro pair that initializes every given adapter with the adapted
/// member.
#define NC_INTERNAL_INIT_TASK_MEMBER(var) var##_adapter(var)
#define NC_INTERNAL_INIT_TASK_MEMBERS(...) \
  NC_INTERNAL_FOREACH_LIST(NC_INTERNAL_INIT_TASK_MEMBER, __VA_ARGS__)

/// @brief Macro pair that couples and synchronizes the given adapters using the
/// references created by NC_INTERNAL_RR_MEMBER_REFS.
#define NC_INTERNAL_COUPLE(var)                       \
  refs.var##_adapter_ref.dynamic_bind(var##_adapter); \
  all_synced &= refs.var##_adapter_ref.synced()
#define NC_INTERNAL_COUPLE_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_COUPLE, __VA_ARGS__)

/// @brief Macro pair that only couples the given adapters using the references
/// created by NC_INTERNAL_RR_MEMBER_REFS.
#define NC_INTERNAL_COUPLE_NOSYNC(var) \
  refs.var##_adapter_ref.dynamic_bind(var##_adapter, false);
#define NC_INTERNAL_COUPLE_NOSYNC_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_COUPLE_NOSYNC, __VA_ARGS__)

/// @brief Macro pair that decouples the given adapters using the references
/// created by NC_INTERNAL_RR_MEMBER_REFS.
#define NC_INTERNAL_DECOUPLE(var) \
  refs.var##_adapter_ref.dynamic_unbind(var##_adapter)
#define NC_INTERNAL_DECOUPLE_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_DECOUPLE, __VA_ARGS__)

/// @brief Macro pair that creates a configuration manager for every given
/// sub-interface.
#define NC_INTERNAL_CONF_MNGR(sname) conf_manager<sname> conf_##sname
#define NC_INTERNAL_CONF_MNGR_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_CONF_MNGR, __VA_ARGS__)

/// @brief Macro pair that connects each specified configuration manager to the
/// reconfiguration region where the module is configured at.
#define NC_INTERNAL_CONF_CONNECT(sname) \
  conf_##sname.connect_to_reconf_zone(configured_rr())
#define NC_INTERNAL_CONF_CONNECT_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_CONF_CONNECT, __VA_ARGS__)

/// @brief Macro pair that performs an module unload on every specified
/// configuration manager and then disconnects it from the reconfiguration
/// region.
#define NC_INTERNAL_CONF_UNLOAD_DC(sname)  \
  unload_module_unprotected(conf_##sname); \
  conf_##sname.disconnect_from_reconf_zone(configured_rr())
#define NC_INTERNAL_CONF_UNLOAD_DC_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_CONF_UNLOAD_DC, __VA_ARGS__)

/// @brief Macro pair that creates an enum values for given sub-interface names
#define NC_INTERNAL_MAKE_AREA(sname) Area_##sname
#define NC_INTERNAL_MAKE_AREAS(...) \
  NC_INTERNAL_FOREACH_LIST(NC_INTERNAL_MAKE_AREA, __VA_ARGS__)

/// @brief Macro pair that creates register and configure functions at a split
/// manager for every given sub-interface.
#define NC_INTERNAL_REG_CONF_SPACE(sname)    \
  template <class Module>                    \
  void register_module_##sname() {           \
    conf_##sname.register_module<Module>();  \
  }                                          \
                                             \
  template <class Module>                    \
  void configure_module_##sname() {          \
    conf_##sname.configure_module<Module>(); \
  }
#define NC_INTERNAL_REG_CONF_SPACE_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_REG_CONF_SPACE, __VA_ARGS__)

/// @brief Macro pair that creates an case in a switch statement that unloads
/// the module for the given sub-interface using the enum values from
/// NC_INTERNAL_MAKE_AREAS.
#define NC_INTERNAL_UNLOAD_CASE(sname) \
  case Area_##sname:                   \
    conf_##sname.unload_module();      \
    break
#define NC_INTERNAL_UNLOAD_CASE_LIST(...) \
  NC_INTERNAL_FOREACH(NC_INTERNAL_UNLOAD_CASE, __VA_ARGS__)

/**
 * Register the members that are associated with this reconfiguration interface.
 * This creates the following classes within the interface class:
 *  rr_base:
 *      This is the base class for every reconfiguration region of that
 * interface class. It contains the members that are exposed to the static
 * design and adapters for it. Furthermore, it contains a struct that has a
 * reference to each of the reconfiguration region's members. A instance of this
 * struct is passed to the respective module for coupling and decoupling
 * adapters. The modules implement the required logic for it.
 *
 *  configurable_base:
 *      This class is the base class for everything that can be configured at a
 * reconfiguration region, which includes modules (see module_base and split
 * manager). It does not contain any member or adapter, but defines pure virtual
 * methods which need to be implemented by deriving types in order to correctly
 * couple and decouple their adapters.
 *
 *  module_base:
 *      This class derives from the configurable_base of this interface.
 *      It contains the members that are exposed to modules and adapters for it.
 *      Here, the actual coupling and decoupling of adapters is implemented.
 *
 *  Further implementations of configurable_base may be created later using
 * NC_SPLIT_INTERFACE(...).
 */
#define NC_INTERFACE_REGISTER(iname, ...)                                      \
  class configurable_base;                                                     \
                                                                               \
  class rr_base : public abstract_reconf_region_base {                         \
    friend class configurable_base;                                            \
                                                                               \
    NC_INTERNAL_RR_MEMBER_LIST(iname, __VA_ARGS__)                             \
                                                                               \
    void nc_couple_member_refs(configurable_base* t, bool sync) {              \
      if (sync)                                                                \
        t->nc_do_couple(member_refs);                                          \
      else                                                                     \
        t->nc_do_couple_no_sync(member_refs);                                  \
    }                                                                          \
                                                                               \
    void nc_decouple_member_refs(configurable_base* t) {                       \
      t->nc_do_decouple(member_refs);                                          \
    }                                                                          \
                                                                               \
   public:                                                                     \
    explicit rr_base(const sc_core::sc_module_name& nm,                        \
                     abstract_module_base* base_module)                        \
        : abstract_reconf_region_base(nm, base_module),                        \
          member_refs(*this),                                                  \
          NC_INTERNAL_INIT_RR_MEMBERS(__VA_ARGS__) {}                          \
                                                                               \
    typedef struct member_ref {                                                \
      NC_INTERNAL_RR_MEMBER_REFS(__VA_ARGS__)                                  \
      explicit member_ref(rr_base& rr)                                         \
          : NC_INTERNAL_RR_MEMBER_REFS_INIT(__VA_ARGS__) {}                    \
    } member_ref_struct;                                                       \
                                                                               \
   private:                                                                    \
    member_ref_struct member_refs;                                             \
  };                                                                           \
                                                                               \
  class configurable_base : public abstract_module_base {                      \
    friend class rr_base;                                                      \
                                                                               \
    void nc_couple(abstract_reconf_region_base* rr,                            \
                   bool sync) override final {                                 \
      sc_assert(m_configured_rr == nullptr);                                   \
      auto rr_ = dynamic_cast<rr_base*>(rr);                                   \
      sc_assert(rr_ != nullptr);                                               \
                                                                               \
      m_configured_rr = rr;                                                    \
      rr_->nc_couple_member_refs(this, sync);                                  \
    }                                                                          \
                                                                               \
    void nc_decouple(abstract_reconf_region_base* rr) override final {         \
      sc_assert(m_configured_rr == rr);                                        \
      auto rr_ = dynamic_cast<rr_base*>(rr);                                   \
      sc_assert(rr_ != nullptr);                                               \
                                                                               \
      m_configured_rr = nullptr;                                               \
      rr_->nc_decouple_member_refs(this);                                      \
    }                                                                          \
                                                                               \
    abstract_reconf_region_base* m_configured_rr = nullptr;                    \
                                                                               \
   protected:                                                                  \
    auto configured_rr() const { return m_configured_rr; }                     \
                                                                               \
    virtual void nc_do_couple(rr_base::member_ref_struct& refs) = 0;           \
                                                                               \
    virtual void nc_do_couple_no_sync(rr_base::member_ref_struct& refs) = 0;   \
                                                                               \
    virtual void nc_do_decouple(rr_base::member_ref_struct& refs) = 0;         \
                                                                               \
   public:                                                                     \
    explicit configurable_base(const sc_core::sc_module_name& nm)              \
        : abstract_module_base(nm) {}                                          \
  };                                                                           \
                                                                               \
  class module_base : public configurable_base {                               \
    friend class rr_base;                                                      \
                                                                               \
    NC_INTERNAL_TASK_MEMBER_LIST(iname, __VA_ARGS__)                           \
                                                                               \
    static const char* default_name() {                                        \
      return sc_core::sc_gen_unique_name(                                      \
          (std::string("module_") + #iname).c_str());                          \
    }                                                                          \
                                                                               \
   public:                                                                     \
    explicit module_base(const sc_core::sc_module_name& nm = default_name())   \
        : configurable_base(nm), NC_INTERNAL_INIT_TASK_MEMBERS(__VA_ARGS__) {} \
                                                                               \
   private:                                                                    \
    void nc_do_couple(rr_base::member_ref_struct& refs) final {                \
      sc_core::sc_event_and_list all_synced;                                   \
      NC_INTERNAL_COUPLE_LIST(__VA_ARGS__)                                     \
      wait(all_synced);                                                        \
    }                                                                          \
                                                                               \
    void nc_do_couple_no_sync(rr_base::member_ref_struct& refs) final {        \
      NC_INTERNAL_COUPLE_NOSYNC_LIST(__VA_ARGS__)                              \
    }                                                                          \
                                                                               \
    void nc_do_decouple(rr_base::member_ref_struct& refs) final {              \
      NC_INTERNAL_DECOUPLE_LIST(__VA_ARGS__)                                   \
    }                                                                          \
  };

#ifndef NC_DONT_HIDE_MODULE_MEMBERS

/// @brief Macro to create a cthread for a module using the libraries overridden
/// "sensitive" member.
#define NC_CTHREAD(func, edge)                                                \
  sensitive.new_args(sc_core::SC_CTHREAD_PROC_);                              \
  this->nc_register_spawn_args(                                               \
      {[this] { func(); }, sc_gen_unique_name(#func), sensitive.get_opts()}); \
  sensitive << edge;

/// @brief Macro to create a method for a module using the overridden
/// "sensitive" member.
#define NC_METHOD(func)                         \
  sensitive.new_args(sc_core::SC_METHOD_PROC_); \
  this->nc_register_spawn_args(                 \
      {[this] { func(); }, sc_gen_unique_name(#func), sensitive.get_opts()});

/// @brief Macro to create a thread for a module using the overridden
/// "sensitive" member.
#define NC_THREAD(func)                         \
  sensitive.new_args(sc_core::SC_THREAD_PROC_); \
  this->nc_register_spawn_args(                 \
      {[this] { func(); }, sc_gen_unique_name(#func), sensitive.get_opts()});

#else /* NC_DONT_HIDE_MODULE_MEMBERS */

/// @brief Macro to create a cthread for a module using the library's distinct
/// "nc_sensitive".
#define NC_CTHREAD(func, edge)                               \
  nc_sensitive.new_args(sc_core::SC_CTHREAD_PROC_);          \
  this->nc_register_spawn_args(                              \
      {[this] { func(); }, #func, nc_sensitive.get_opts()}); \
  nc_sensitive << edge;

/// @brief Macro to create a method for a module using the library's distinct
/// "nc_sensitive".
#define NC_METHOD(func)                            \
  nc_sensitive.new_args(sc_core::SC_METHOD_PROC_); \
  this->nc_register_spawn_args(                    \
      {[this] { func(); }, #func, nc_sensitive.get_opts()});

/// @brief Macro to create a thread for a module using the library's distinct
/// "nc_sensitive".
#define NC_THREAD(func)                            \
  nc_sensitive.new_args(sc_core::SC_THREAD_PROC_); \
  this->nc_register_spawn_args(                    \
      {[this] { func(); }, #func, nc_sensitive.get_opts()});

#endif /* NC_DONT_HIDE_MODULE_MEMBERS */

/**
 * Create a sub-interface from a base interface that contains the given members
 * of the base interface. This will create a new implementation of the base
 * interface's configurable_base, but this time only couple and decouple the
 * members that were specified for this sub-interface.
 */
#define NC_SPLIT_INTERFACE(sub_interface, base_interface, ...)                 \
  class sub_interface : public nc_interface {                                  \
   public:                                                                     \
    typedef base_interface::rr_base rr_base;                                   \
    typedef base_interface::configurable_base configurable_base;               \
                                                                               \
    class module_base : public configurable_base {                             \
      NC_INTERNAL_TASK_MEMBER_LIST(base_interface, __VA_ARGS__)                \
                                                                               \
      friend class base_interface::rr_base;                                    \
                                                                               \
      void nc_do_couple(rr_base::member_ref_struct& refs) override {           \
        sc_core::sc_event_and_list all_synced;                                 \
        NC_INTERNAL_COUPLE_LIST(__VA_ARGS__)                                   \
        wait(all_synced);                                                      \
      }                                                                        \
                                                                               \
      void nc_do_couple_no_sync(rr_base::member_ref_struct& refs) override {   \
        NC_INTERNAL_COUPLE_NOSYNC_LIST(__VA_ARGS__)                            \
      }                                                                        \
                                                                               \
      void nc_do_decouple(rr_base::member_ref_struct& refs) override {         \
        NC_INTERNAL_DECOUPLE_LIST(__VA_ARGS__)                                 \
      }                                                                        \
                                                                               \
     public:                                                                   \
      explicit module_base(const sc_core::sc_module_name& nm)                  \
          : configurable_base(nm),                                             \
            NC_INTERNAL_INIT_TASK_MEMBERS(__VA_ARGS__) {}                      \
    };                                                                         \
                                                                               \
    constexpr static double load_speed_MBps = base_interface::load_speed_MBps; \
  }

/**
 * Register a complementary group of sub-interfaces of a given base interface.
 * The sub-interface all need to be derived from the same base interface and no
 * member of the base interface shall be contained by more than one of the
 * specified sub-interfaces. This macro creates a split manager for the base
 * interface, which has a configuration manager for every specified
 * sub-interface and provides functions to register and configure modules of
 * appropriate type at them. It also implements the base interface's
 * configurable_base, but does not couple or decouple any adapters since this is
 * done by the respective configuration managers.
 */
#define NC_SPLIT_REGISTER(base_interface, ...)                                \
  class base_interface##_manager : public base_interface::configurable_base,  \
                                   public split_manager_base {                \
   public:                                                                    \
    NC_INTERNAL_CONF_MNGR_LIST(__VA_ARGS__)                                   \
   private:                                                                   \
    typedef base_interface::rr_base rr_base;                                  \
                                                                              \
    void nc_do_couple(rr_base::member_ref_struct& refs) override {            \
      (void)refs;                                                             \
      NC_INTERNAL_CONF_CONNECT_LIST(__VA_ARGS__)                              \
    }                                                                         \
                                                                              \
    void nc_do_couple_no_sync(rr_base::member_ref_struct& refs) override {    \
      /* nothing to couple here, but we connect our conf_manager to the rr */ \
      nc_do_couple(refs);                                                     \
    }                                                                         \
                                                                              \
    void nc_do_decouple(rr_base::member_ref_struct& refs) override {          \
      (void)refs;                                                             \
      /* nothing to decouple here, don't disconnect conf_manager yet, will be \
       * needed for unload */                                                 \
    }                                                                         \
                                                                              \
    void on_activate() override {                                             \
      /* TODO: preloads should be activated here */                           \
    }                                                                         \
                                                                              \
    void on_deactivate() override {                                           \
      NC_INTERNAL_CONF_UNLOAD_DC_LIST(__VA_ARGS__)                            \
    }                                                                         \
                                                                              \
   public:                                                                    \
    enum Area { NC_INTERNAL_MAKE_AREAS(__VA_ARGS__) };                        \
                                                                              \
    explicit base_interface##_manager(                                        \
        const sc_core::sc_module_name& nm =                                   \
            (std::string(#base_interface) + "_manager").c_str())              \
        : configurable_base(nm) {}                                            \
                                                                              \
    NC_INTERNAL_REG_CONF_SPACE_LIST(__VA_ARGS__)                              \
                                                                              \
    void unload(Area a) {                                                     \
      switch (a) { NC_INTERNAL_UNLOAD_CASE_LIST(__VA_ARGS__) }                \
    }                                                                         \
  }

#endif  // NC_DEFINITIONS_H_
