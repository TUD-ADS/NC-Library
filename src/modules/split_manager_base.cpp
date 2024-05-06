/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#include "split_manager_base.h"

#include "region/conf_manager.h"

void split_manager_base::unload_module_unprotected(conf_manager_base& mngr) {
  mngr.unload_module_unprotected();
}
