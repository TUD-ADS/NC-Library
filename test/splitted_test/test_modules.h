/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include "bw_if_dummy.h"
#include "splitted_interface.h"

NC_MODULE(module1, base_interface) {
 public:
  SC_HAS_PROCESS(module1);
  NC_CTOR(module1, base_interface) {
    NC_METHOD(calculation)
    sensitive << input << input2;
    dont_initialize();
  }

  void calculation() {
    output.write(input.read() + input2.read());
    output2.write(input.read() - input2.read());
  }
};

NC_MODULE(module2_1a, sub_interface1) {
  SC_HAS_PROCESS(module2_1a);
  NC_CTOR(module2_1a, sub_interface1) {
    NC_METHOD(calculation_1a)
    sensitive << input;
    dont_initialize();
  }

  void calculation_1a() { output.write(input.read()); }
};

NC_MODULE(module2_1b, sub_interface1) {
  SC_HAS_PROCESS(module2_1b);
  NC_CTOR(module2_1b, sub_interface1) {
    NC_METHOD(calculation_1b)
    sensitive << input;
    dont_initialize();
  }

  void calculation_1b() { output.write(2 * input.read()); }
};

NC_MODULE(module2_2, sub_interface2) {
  SC_HAS_PROCESS(module2_2);
  NC_CTOR(module2_2, sub_interface2) {
    NC_METHOD(calculation)
    sensitive << input2;
    dont_initialize();
  }

  void calculation() { output2.write(4 * input2.read()); }
};

#endif  // NESTED_RECONF_TEST_TASKS_H
