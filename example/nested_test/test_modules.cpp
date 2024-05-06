//
// Created by alex on 11.10.23.
//

#include "test_modules.h"

/// reflector module

void reflector_module::reflect() { output.write(input.read()); }
/// add one module

void add_one_module::add_one_in() {
  input.read();
  rz_in.write(input.read() + 1);
}

void add_one_module::forward_out() { output.write(rz_out.read()); }

add_one_module &add_one_module::next_add() {
  m_rz.configure_module<add_one_module>();
  return *m_rz.get_module<add_one_module>();
}

void add_one_module::finish_reflect() {
  m_rz.configure_module<reflector_module>();
}

void add_one_module::configure(int remaining_adds) {
  if (remaining_adds > 0) {
    auto &module = next_add();
    module.configure(--remaining_adds);
  } else {
    finish_reflect();
  }
}

/// base module

void base_module::configure(int number_of_adds) {
  if (number_of_adds > 0) {
    auto &module = next_add();
    module.configure(--number_of_adds);
  } else {
    finish_reflect();
  }
}

add_one_module &base_module::next_add() {
  m_rz.configure_module<add_one_module>();
  return *m_rz.get_module<add_one_module>();
}

void base_module::finish_reflect() {
  m_rz.configure_module<reflector_module>();
}
