/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include "abstract_reconf_region_base.h"

#include "modules/abstract_module_base.h"

abstract_reconf_region_base::abstract_reconf_region_base(
    const sc_core::sc_module_name &name, abstract_module_base *base_module)
    : sc_core::sc_module(name), m_base_module(base_module) {
  if (base_module != nullptr) {
    base_module->register_nested_rr(this);
  }
}
