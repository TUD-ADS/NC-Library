# Writing reconfiguration zone adapters pairs
We cannot dynamically change which signal an `sc_in` is bound to or which `tlm_initiator_socket` can send to our `tlm_target_socket` now (without integrating additional hardware abstractions like routers, etc.).     
This is why the core idea of the reconfiguration process, consists of two adapters (an adapter pair) for every member of the interface. One of them is _statically_ bound to the static modules outside the reconfiguration zone and the other one is also _statically_ bound to a module inside the reconfiguration zone (RZ). To be more precise, there is one adapter bound to every module inside the RZ (so-called module-side adapter), but exactly one adapter that binds to the "outside world" (so-called rz-side adapter).   
The adapters job is to propagate any interactions with them to the other adapter of the pair. An adapter pair thereby always consists out of the rz-side adapter of the corresponding RZ and on of the module-side adapters. Which module-side adapter is in the pair, depends on which module is currently configured.  
Since these "interactions" can be of many kinds, we need different adapters to correctly relay the behaviour of the types the bind to.  
However, they all need to adhere a common structure, which is described below.  
You may notice that there is no common base class that describes the required structure. This is because the types of the functions arguments change for different adapters, and it is rather hard to express this with templates due to the way how [adapter pairs a defined](#defining-an-adapter-pair)


## rz-side
### Required Structure
```cpp
/// The SystemC type that the adapters bridge through the RZ interface.
class core_type;

/// The adapter that statically connects to the  
class module_side_adapter;

struct rz_side_adapter, public sc_core::sc_module {
    rz_side_adapter_base(&)
    {
        // do static bind here
    }
    
    void before_end_of_elaboration() override
    {
        // do static binds here that need to happen after the static modules
        // bound to the adapted target in the rz
    }

    virtual void dynamic_bind(typename adapter_traits<T>::module_adapter&)
    {
        
    }

    virtual void dynamic_unbind(typename adapter_traits<T>::module_adapter&)
    {
        
    }
};
```

### Locking
The rz_adapter receives a reference to the RZ that it is attached to as the first argument.
Using that, it can create reconf_locks which work like scoped locks or manually start and end interactions using the static interface of reconf_lock.
In the bind functions a pointer to the rz can be passed to the module_adapter.
However, it is advised to just make the both adapter classes friends of each other, so that the module_adapter can use the reference stored in the rz_adapter.
Thus, the module_adapter always only has to check whether the pointer to the currently active rz_adapter is valid.


## module-side
```cpp
/// 
class module_side_adapter;

struct module_side_adapter, public sc_core::sc_module {
    module_side_adapter(T&)
    {
        // do static bind here
    }
    
    void before_end_of_elaboration() override
    {
        // do static binds here that need to happen after the static modules
        // bound to the adapted target in the module
    }
};
```

## defining an adapter pair

