# Basic Working Principle / Where to start reading the code
The best way to understand how the library works is to read the thesis or more precisely chapter "4 Design And Implementation".
However, here is a quick recap of where you can find what:

The directory `region` contains the `abstract_reconf_region` definitions, which cares about everything that can be done without proper knowledge of the interfaces (primarily reconfiguration protection).
It also contains the `conf_manager`, which implements the actual reconfiguration logic and the `reconf_region`, which combines the logic of the `conf_manager` with the members inherited from the interface classes.

In directory `modules` you can find the `abstract_module_base`, which in turn handles everything for modules that can be done without proper knowledge of the interfaces (a.k.a. process management and handling nested reconfiguration regions).
You can also find the `split_manager_base` here, which is needed for split reconfiguration as explained in the paper.

The `abstract_reconf_region` and the `abstract_module_base` will be implemented by an `rr_base` and an `configurable_base` per reconfiguration interface respectively.
These are created by the macro `NC_INTERFACE_REGISTER(...)` defined in file `nc_definitions.h`.
The `configurable_base` does not yet contain any interface members, since it is also the base class for potential split manager.
The class `module_base`, which is also created in the interface class by the above macro further extends the `configurable_base` about the necessary members and the coupling and decoupling logic for them.
Any modules written by the developer need to inherit from `module_base`, which is exactly what the macro `NC_MODULE(...)` does.

Directory `processes` contains helper classes for registering sensitivities and the kind of processes for our modules in a SystemC-like fashion, a.k.a. using a `sensitive` member and macros like `NC_THREAD` etc.

Directory `locking` contains the `reconf_lock`, which enables adapters to block prohibit a reconfiguration of the current module.

At last, directory `forwarding` contains the currently available member adapters and the `adapter_traits` structs used to select them.
Furthermore, provides the struct `nc_decouple_value` the correct value that should be set on ports when no module is configured.

# Current Limitations/Open Issues
The existing macros in `nc_interface.h` currently only support up to 10 interface members.
These should be extended to support up to 50 members.
It is probably a good idea to move the macros `NC_INTERNAL_CALL*` in a new file for this.

The reconfiguration speed is as of now defined in the interface using the macro `NC_LOAD_SPEED_MBPS(speed)`.
It should be defined per reconfiguration region with a default value similar as it is done with the modules size.
This would allow multiple reconfiguration regions of the same interface, but with different reconfiguration speeds.

As already mentioned there are currently only adapters implemented for the SystemC prot types `sc_in<T>` and `sc_out<T>` as well as the TLM sockets.
Also, the TLM adapters lack proper template arguments and are at the moment only able to forward transactions for default template arguments.  
The following section shall provide a concise description of how the existing adapters work, so that new ones can be developed.

# Writing Adapters (link to other document)
For every member type, two adapters are needed.
One for the module side and another for the static side, a.k.a. the reconfiguration region.
The general idea is to use the interface member on the static side normally, register what happens with the adapter on the static side and replicate the interactions from the static side on the member in the module using the adapter on the module side.
The whole procedure can also work in reverse, where the adapter on the static side imitates the modules actions or both directions at once, depending on what is appropriate for the adapted type.

The adapters interface depends on the type of the interface member, since the adapter shall provide a static binding partner too it.
For example, the adapter for `tlm_target_socket` on the static side needs to implement the `tlm_fw_transport_if` in order to be able to "listen" for transactions on the socket.
In turn, the module adapter for this type will have a `tlm_target_socket` with which it can propagate transactions detected by the other adapter to the `tlm_target_socket` that is exposed to the module.
It thus will have to implement the `tlm_bw_transport_if` in order to use the target socket and also relays calls on the backwards path in the same manner.

Due to the heterogeneous interfaces, the thesis describes some basic rules that all adapter pairs are expected to follow:
1. The first argument to an adapter's constructor is a reference to the interface member that it shall statically bind to.
2. Adapters on the static side receive a reference to the according reconfiguration region as a second constructor argument.
3. Adapters on the static side provide the functions `void dynamic_bind(T&, bool)` and `void dynamic_unbind(T&, bool)`, which are used to establish and abolish the forwarding. `T` describes the type of the other adapter with which the forwarding shall be established/abolished. The second argument is a flag indicating whether the adapters should synchronize. The unbind function shall also never block.
4. Adapters on the static side need to implement the function `sc_event& synced()on the static side, which returns a reference to an event that indicates whether the synchronization process of the adapters has completed.

The synchronization is not relevant for TLM socket adapters, since they have no `value` in the way a SystemC sc_in/sc_out port has one.
However, when a sc_in port is member of an interface and a modules gets configured, the value of the port might no longer be the default value from the simulation start.
Thus, the adapters should propagate this changed value to the module, even though the value did not change, only the configured module changed.
The update process makes use of events and will block for at least one delta-cycle.
This again would make problems when we were to preload a module with this member, since at the time in the simulation where the preloading happens, SystemC does not allow to wait for events yet.
Due to that the synchronization can be explicitly be skipped by passing `false` as second argument to `dynamic_bind`.

The synchronization is currently only relevant for ports of type `sc_in`, since a module could not have written to a `sc_out` yet, when its members are just coupled now.
However, there is something else we need to think of for SystemC port types that transfer values out of the reconfiguration region.
When we connect a `sc_signal` with default template arguments to a port of the reconfiguration region, it can only be written to by one process, since the default policy for the signal is `SC_ONE_WRITER`.
But when a new module is configured in the reconfiguration region, then this module will also have new processes, which propagate (with the help of teh adapters) a value to the static side and try to drive the signal.
This will of course fail.
A simple solution is of course to just use a signal with policy `SC_MANY_WRITERS`, but having the number of drivers checked can be a quite good help for complex designs, and we do not want to impose this limitation on signals of the static design.
Thus, we create an additional thread which is run by the adapter on the static side.
Whenever a value needs to be written, the adapter on the module side only notifies the adapter on the static side, so that it can fetch the new value from the module adapter and write it on the signal.
An implementation of this principle can be found in the adapters `rr_port_adapter<sc_out<T>>` and `module_port_adapter<sc_out<T>>`.
