/**
 * Copyright 2024 The NC-Library Authors. All rights reserved.
 *
 * Licensed under Apache License Version 2.0.
 *
 */

#ifndef NESTED_RECONF_TEST_TASKS_H
#define NESTED_RECONF_TEST_TASKS_H

#include "my_interface.h"

NC_MODULE(adder, my_interface){

    NC_CTOR(adder, my_interface){NC_METHOD(do_add) sensitive << in1 << in2;
}

void do_add() { out.write(in1.read() + in2.read()); }
}
;

NC_MODULE(mul, my_interface){

    NC_CTOR(mul, my_interface){NC_METHOD(do_add) sensitive << in1 << in2;
}

void do_add() { out.write(in1.read() * in2.read()); }
}
;

#endif  // NESTED_RECONF_TEST_TASKS_H
