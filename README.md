# Table Of Contents
- [Table Of Contents](#table-of-contents)
- [Using the library](#using-the-library)
  - [Introductory Example](#introductory-example)
    - [Defining An Interface](#defining-an-interface)
    - [Writing The Modules](#writing-the-modules)
    - [Using The Reconfiguration Region](#using-the-reconfiguration-region)
  - [Developer Notes](#developer-notes)
  - [Advanced Examples](#advanced-examples)
    - [Changing Module Sizes](#changing-module-sizes)
    - [Preloading Modules](#preloading-modules)
    - [Nesting Reconfiguration Regions](#nesting-reconfiguration-regions)
    - [Splitting Interfaces](#splitting-interfaces)
- [Integration In Your Project](#integration-in-your-project)
  - [CMake](#cmake)
- [Documentation For Library Developers](#documentation-for-library-developers)


# Using the library

## Introductory Example

### Defining An Interface
Before creating am reconfiguration region we first need to define how which interface their reconfigurable modules (from here on referred to as*modules*) will have with the static design.
To do this, we create an interface class using the macro `NC_INTERFACE(name)`. There we list all SystemC ports and TLM sockets that shall be part of the interface as members of the class.
To create the required structures for communication forwarding between the static design and the modules, all members of the interface need to be registered at the interface using the macro `NC_INTERFACE_REGISTER(interface_name, members ...)`.
Additionally, we need to specify the reconfiguration speed of our interface. This will influence how long it takes to configure different modules. It can be specified by the macro `NC_LOAD_SPEED_MBPS(speed)`.

```cpp
NC_INTERFACE(my_interface) {
    sc_core::sc_in<float> in1, in2;
    sc_core::sc_out<float> out;

    NC_LOAD_SPEED_MBPS(128);

    NC_INTERFACE_REGISTER(my_interface, in1, in2, out);
};
```

### Writing The Modules
Using the just defined interface and the macro `NC_MODULE(module_name, interface_name)`, we can create modules for our reconfiguration region. An example is shown here and explained below:

```cpp
NC_MODULE(adder, my_interface) {

    NC_CTOR(adder, my_interface)
    {
        NC_METHOD(do_add)
        sensitive << in1 << in2;
    }

    void do_add()
    {
        out.write(in1.read() + in2.read());
    }
};
```

The here shown module represents an adder module, that calculates the sum of the values of both input ports (`in1` and `in2`) and writes it to the output port (`out`).
For this the module needs one method, that is run whenever any of the inputs changes.
The modules constructor can be specified using the macro `NC_CTOR(module_name, interface_name)`
The method and its sensitivities are defined analogous to how one would do it for a method in a normal `sc_module`, except that the macro `NC_METHODE` is used instead of `SC_METHODE`.
We also don't call `dont_initialize`, which will result in our method being run immediately on configuration of the module.
A closer look at why we do not use `dont_initialize` here is explained in [this section](#documentation-for-library-developers).
The rest of the module description is default SystemC.

### Using The Reconfiguration Region

Before we integrate the reconfiguration region in our design, we of course should define more than one module.
Otherwise, it would not really make sense to make ourselves the hustle of reconfiguration if we only ever employ one module in it.
Thus, we assume that we have also defined a module `mul` for the above interface, which behaves similar to the already known `adder` module, except that it multiplies its input signals, instead of adding them.

With these two modules at hand, we will now add a reconfiguration region of the same interface to our static design.
This can be done by defining it as a member of one of our static modules:
```cpp
#define ARRAY_SIZE 3

SC_MODULE(top) {
    reconf_region<my_interface> rr {"rr"};
    sc_core::sc_signal<float> sig_in1, sig_in2, sig_out;
    sc_core::sc_out<float> a, b;
    sc_core::sc_in<float> result;

    float a_values[ARRAY_SIZE] = { 4.1f, 2.87f, 7.3f };
    float b_values[ARRAY_SIZE] = { 5.2f, 3.12f, 3.74f };

    SC_CTOR(top)
    {
        rr.register_module<adder>();
        rr.register_module<mul>();

        rr.in1.bind(sig_in1);
        rr.in2.bind(sig_in2);
        rr.out.bind(sig_out);

        a.bind(sig_in1);
        b.bind(sig_in2);
        result.bind(sig_out);

        SC_THREAD(run)
    }

    void run()
    {
        float sums[ARRAY_SIZE];
        float scalar_product = 1.f;

        rr.configure_module<adder>();

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            a.write(a_values[i]);
            b.write(b_values[i]);

            wait(1, sc_core::SC_NS);

            sums[i] = result.read();
        }

        rr.configure_module<mul>();

        for (int i = 0; i < ARRAY_SIZE; i++)
        {
            a.write(scalar_product);
            b.write(sums[i]);

            wait(1, sc_core::SC_NS);

            scalar_product = result.read();
        }

        std::cout << "Result is: " << scalar_product << std::endl;
    }
};
```

Let's break down what happens in the above code snippet:

**Member**
The static module `top` has a reconfiguration region of our interface `my_interface`.
Additionally, it contains two output and one input port as well as signals to connect them to those of the reconfiguration region.
Usually, we would relocate these ports in another static module and only have the connecting signals in the top-level module, but to keep it simple we put everything in the top-level module here.

**Behavior**
The top-level module's constructor looks similar to already known SystemC constructors. It connects the ports and signals and defines a thread `run` without sensitivities, which will be executed on simulation start. However, since `sc_module` are constructed during elaboration time, this is also the point where our reconfigurable modules need to be registered at the reconfiguration region, as shown in right at the beginning of the constructor.

The module's thread `run` then configures the `mul` module and uses it to multiply some float values. Afterwards, the reconfiguration region is configured with the `adder` module to build the sum of the previously calculated products. The function then exits since no more work is to be done.

The full code of this example can be found in `test/readme_example`.

## Developer Notes

Due to the way how the processes were implemented, only signals with `SC_MANY_WRITERS` or `SC_UNCHECKED_WRITERS` policy can be used within modules. Any signals connected to the reconfiguration region can however also make use the policy `SC_ONE_WRITER`.

Furthermore, consider that when your module is load, any ingoing ports which are driven from the static side might already have a value different from their default value, since it could have changed before the current module was load.
It is therefore worth to reconsider, whether a call to `dont_initialize()` really should be performed.

It is also possible to reuse the implementation of a static module and derive a reconfigurable one (a.k.a. a module) from it under the following conditions:
1. The new module, which inherits from the existing static module, also need to specify an appropriate reconfiguration interface, for example using the `NC_MODULE(...)` macro shown above the above.
2. All communication across reconfiguration boundaries has to be handled by the ports and sockets specified in the modules reconfiguration interface.
3. All processes need to be spawned using the previously described macros `NC_THREAD`,
`NC_METHOD` or `NC_CTHREAD`. If the static module contains SystemC processes definitions (`SC_THREAD`, `SC_METHOD` or `SC_CTHREAD`), then they need to be moved to the modules and replaced by the according macros provided by the library.

## Advanced Examples
### Changing Module Sizes
As seen in the [basic example](#introductory-example), changing the reconfiguration region's modules takes quite some time.
To be more precise, the time it takes to configure a new module is determined by the new module's size and the reconfiguration speed that we defined in the interface earlier.
When there is already a module configured, it will be automatically unloaded before the new module is configured.
This does not consume simulation time, it only takes one delta cycle.
Every module's size is set to 1024 byte by default, which results in a configuration delay of around `1024 byte / 128 MiBps = 1024 byte / (128 * 1024^2 byte ) / s  = 7.63 µs`.
The size of each module can be set individually by overwriting the function `size_t get_bitstream_size_bytes();`.

### Preloading Modules
It was already mentioned that a reconfiguration procedure takes some time.
This of course also applies to the initial configuration of a module if it is performed during simulation time with `configure_module<Module>()`.
However, every reconfiguration region can also have a module already configured on simulation start, which allows an application to immediately use it.
To do this, one needs to register the module for preloading using the function `reconf_region::preload_module<Module>()`, where `Module` is a module type that already needs to be registered at the reconfiguration region.
For example, it could be called in the top module's constructor from the example before after the registration is done, but also at any later point during the elaboration time.
Obviously, it needs to happen before the simulation started.

### Nesting Reconfiguration Regions
Since module and reconfiguration regions are `sc_modules` themselves, a module can also define a reconfiguration region of arbitrary interface as one of its members.
In order for the reconfiguration region to behave correctly it needs to be given the pointer to the module that contains it as second constructor argument.
This is used to ensure that the nested reconfiguration region is properly cleared, when this module is unload.
Modules can also be registered for preloading at nested reconfiguration regions.
The preloading is then performed whenever the module that contains the nested reconfiguration region is configured.

### Splitting Interfaces
Another feature is the splitting of reconfiguration interfaces which enables split configuration of reconfiguration regions.
The idea is that a base interface can be split in multiple sub-interfaces which contain non-overlapping subsets of the base interface's members.

As an example, consider the following base interface:
```cpp
NC_INTERFACE(base_interface) {
    sc_core::sc_in<float> in1, in2;
    sc_core::sc_out<float> out1, out2;

    NC_LOAD_SPEED_MBPS(380);
    NC_INTERFACE_REGISTER(base_interface, in1, in2, out1, out2);
};
```

It is split into `sub_interface1` containing the ports `in1` and `out1`:
```cpp
NC_SPLIT_INTERFACE(sub_interface1, base_interface, in1, out1);
```

And `sub_interface2` containing the ports `in2` and `out2`:
```cpp
NC_SPLIT_INTERFACE(sub_interface2, base_interface, in1, out1);
```


Note that their interface definitions are much shorter, since the type of the members can be inferred from the specified base interface.
With this we split our base interface into two identical smaller interfaces, but we could also have split it into more and non-identical interfaces.
We can develop modules for these sub-interfaces in the same way that it was shown in the [basic example](#introductory-example).
In order to configure them however, we also need to register our sub-interfaces together with the base interface.
Our complete interface file would thus look like the following:
```cpp
NC_INTERFACE(base_interface) {
    sc_core::sc_in<float> in1, in2;
    sc_core::sc_out<float> out1, out2;

    NC_LOAD_SPEED_MBPS(380);
    NC_INTERFACE_REGISTER(base_interface, in1, in2, out1, out2);
};

NC_SPLIT_INTERFACE(sub_interface1, base_interface, in1, out1);

NC_SPLIT_INTERFACE(sub_interface2, base_interface, in1, out1);

NC_SPLIT_REGISTER(base_interface, sub_interface1, sub_interface2);
```

The last line creates as so-called *split manager* of type `base_interface_manager`.
It can be configured like any normal module at a reconfiguration region of the base interface.
When the split manager is configured, no ports of the reconfiguration region are coupled yet, since the split manager does not contain the necessary interface members itself.
However, we can use it to configure one of our module for a split interface at the according part pf the base reconfiguration region.
For example, if we were to develop the module `sub_module_1`  for our `sub_interface1`, we could then configure it using: `base_interface_manager::configure_module_sub_interface1<sub_module_1>();`.
The instance of the split manager can be retrieved beforehand using `reconf_region::get_module<base_interface_manager>();`.


# Integration In Your Project
The library currently only supports building from source.
There are no prebuilt binaries available.

This version of the library was build with SystemC 2.3.3, and it is advised to build your project with the same SystemC standard.
It is planned to upgrade to System 2.3.4 soon.

## CMake
Your SystemC installation is expected in the default install path (`/usr/local`).
If you installed it somewhere else, you can add a file called `CMakePaths.config` in the root directory of the library repository with the following content:
```cmake
set(SYSTEMC_ROOT /path/to/systemc)
```

Assuming the library is located in the `nested_reconf_dir`, you can add the library as follows to your project:
```cmake
add_subdirectory(/path/to/nested_reconf_dir)

project(your_project_name)
add_executable(${PROJECT_NAME} ...your source files...)
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
```

# Documentation For Library Developers
If you want to contribute to this library, additional information on how it works can be found [here](docs/dev_docs.md).
