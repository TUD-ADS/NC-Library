/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */
#ifndef LOCKING_RECONF_LOCK_H_
#define LOCKING_RECONF_LOCK_H_

#include "region/reconf_region.h"

/**
 * The reconf_lock can be used by adapters for in sections during which no
 * reconfiguration shall happen (e.g. during transactions). It can be either
 * used by calling the static functions for locking and unlocking or one can
 * construct an instance of a reconf_lock given the reference to a
 * reconfiguration region. The reconf_lock's behavior is inspired by the
 * std::scoped_lock for concurrency. When a instance of it is created, it
 * immediately signals the start of a transaction to the reconfiguration region.
 * The transaction only ends, when the reconf_lock instance is destructed, which
 * preferably happens when the scope of the protected function/section is left.
 */
class reconf_lock {
  /// @brief Reference to the reconfiguration region in which the transaction
  /// was started.
  abstract_reconf_region_base& m_rr;

 public:
  /**
   * Create an instance, thereby signalling the start of a transaction to the
   * given reconfiguration region.
   *
   * @param rr the reconfiguration region to which the lock shall signal
   */
  explicit reconf_lock(abstract_reconf_region_base& rr) : m_rr(rr) {
    m_rr.start_transaction();
  }

  /**
   * Destructs the reconf_lock, thereby signalling the end of the transaction.
   */
  virtual ~reconf_lock() { m_rr.end_transaction(); }

  /**
   * Signal the start of a transaction to the given reconfiguration region.
   *
   * @param rr the reconfiguration region to which the lock shall signal
   */
  static void lock_static(abstract_reconf_region_base& rr) {
    rr.start_transaction();
  }

  /**
   * Signal the end of a transaction to the given reconfiguration region.
   *
   * @param rr the reconfiguration region to which the lock shall signal
   */
  static void unlock_static(abstract_reconf_region_base& rr) {
    rr.end_transaction();
  }
};

#endif  // LOCKING_RECONF_LOCK_H_
