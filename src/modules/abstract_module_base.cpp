/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include <sysc/kernel/sc_spawn.h>

#include <sstream>

#include "abstract_module_base.h"
#include "nc_report.h"

void abstract_module_base::register_nested_rr(
    abstract_reconf_region_base* region) {
  auto it = std::find(m_nested_regions.begin(), m_nested_regions.end(), region);

  if (it != m_nested_regions.end()) {
    std::stringstream ss;
    ss << "Reconfiguration region " << region->name()
       << " is already registered.";
    SC_REPORT_ERROR(name(), ss.str().c_str());
  }

  m_nested_regions.push_back(region);
}

void abstract_module_base::nc_activate() {
  this->on_activate();

  for (auto& args : m_nc_proc_spawn_args) {
    m_nc_proc_handles.push_back(
        sc_spawn(args.func, args.name, args.opts.get()));
  }

  for (auto& region : m_nested_regions) {
    region->do_preload();
  }
}

void abstract_module_base::nc_deactivate()

{
  this->on_deactivate();

  for (auto& p : m_nc_proc_handles) {
    p.kill();
  }
  m_nc_proc_handles.clear();

  for (auto& region : m_nested_regions) {
    region->unload_module();
  }
}

void abstract_module_base::nc_register_spawn_args(spawn_args&& args) {
  m_nc_proc_spawn_args.emplace_back(args);
}

abstract_module_base::abstract_module_base(const sc_core::sc_module_name& nm)
    : sc_module(nm) {}
