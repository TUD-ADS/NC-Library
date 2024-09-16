#####################################################
## Tests
#####################################################

enable_testing()

add_executable(general_test pure_tlm_interface/top_general_test.cpp)
target_include_directories(general_test PRIVATE . general_test)
target_link_libraries(general_test SystemC::systemc NC_LIB)
add_test(NAME general_test COMMAND general_test)

add_executable(reconf_during_transaction pure_tlm_interface/top_reconf_during_transaction.cpp)
target_include_directories(reconf_during_transaction PRIVATE . reconf_during_transaction)
target_link_libraries(reconf_during_transaction SystemC::systemc NC_LIB)
add_test(NAME reconf_during_transaction COMMAND reconf_during_transaction)

add_executable(transaction_during_reconf pure_tlm_interface/top_transaction_during_reconf.cpp)
target_include_directories(transaction_during_reconf PRIVATE . transaction_during_reconf)
target_link_libraries(transaction_during_reconf SystemC::systemc NC_LIB)
add_test(NAME transaction_during_reconf COMMAND transaction_during_reconf)

add_executable(ports_and_sockets_test port_tests/top_ports_and_sockets_test.cpp)
target_include_directories(ports_and_sockets_test PRIVATE . ports_and_sockets_test)
target_link_libraries(ports_and_sockets_test SystemC::systemc NC_LIB)
add_test(NAME ports_and_sockets_test COMMAND ports_and_sockets_test)

add_executable(double_write write_test/top_double_write.cpp)
target_include_directories(double_write PRIVATE . double_write)
target_link_libraries(double_write SystemC::systemc NC_LIB)
add_test(NAME double_write COMMAND double_write)

add_executable(thread_test thread_test/top_thread_test.cpp)
target_include_directories(thread_test PRIVATE . thread_test)
target_link_libraries(thread_test SystemC::systemc NC_LIB)
add_test(NAME thread_test COMMAND thread_test)

add_executable(cthread_test cthread_test/top_cthread_test.cpp)
target_include_directories(cthread_test PRIVATE . cthread_test)
target_link_libraries(cthread_test SystemC::systemc NC_LIB)
add_test(NAME cthread_test COMMAND cthread_test)

add_executable(method_test method_test/top_method_test.cpp)
target_include_directories(method_test PRIVATE . method_test)
target_link_libraries(method_test SystemC::systemc NC_LIB)
add_test(NAME method_test COMMAND method_test)

add_executable(splitted_test splitted_test/top_splitted_test.cpp)
target_include_directories(splitted_test PRIVATE . splitted_test)
target_link_libraries(splitted_test SystemC::systemc NC_LIB)
add_test(NAME splitted_test COMMAND splitted_test)

add_executable(nested_test nested_test/top_nested_test.cpp)
target_include_directories(nested_test PRIVATE . nested_test)
target_link_libraries(nested_test SystemC::systemc NC_LIB)
add_test(NAME nested_test COMMAND nested_test)

add_executable(nested_unload_test nested_unload/top_nested_unload_test.cpp)
target_include_directories(nested_unload_test PRIVATE . nested_unload_test)
target_link_libraries(nested_unload_test SystemC::systemc NC_LIB)
add_test(NAME nested_unload_test COMMAND nested_unload_test)

add_executable(immediate_transaction_test immediate_transaction_test/top_immediate_transaction_test.cpp)
target_include_directories(immediate_transaction_test PRIVATE . immediate_transaction_test)
target_link_libraries(immediate_transaction_test SystemC::systemc NC_LIB)
add_test(NAME immediate_transaction_test COMMAND immediate_transaction_test)

add_executable(inner_signal_test inner_signal_test/top_inner_signal_test.cpp)
target_include_directories(inner_signal_test PRIVATE . inner_signal_test)
target_link_libraries(inner_signal_test SystemC::systemc NC_LIB)
add_test(NAME inner_signal_test COMMAND inner_signal_test)

add_executable(inner_signal_many_writers_test inner_signal_many_writers_test/top_inner_signal_many_writers_test.cpp)
target_include_directories(inner_signal_many_writers_test PRIVATE . inner_signal_many_writers_test)
target_link_libraries(inner_signal_many_writers_test SystemC::systemc NC_LIB)
add_test(NAME inner_signal_many_writers_test COMMAND inner_signal_many_writers_test)

add_executable(preload_test preload_test/top_preload_test.cpp)
target_include_directories(preload_test PRIVATE . preload_test)
target_link_libraries(preload_test SystemC::systemc NC_LIB)
add_test(NAME preload_test COMMAND preload_test)

add_executable(vector_test vector_test/top_vector_test.cpp)
target_include_directories(vector_test PRIVATE . vector_test)
target_link_libraries(vector_test SystemC::systemc NC_LIB)
add_test(NAME vector_test COMMAND vector_test)


add_subdirectory(readme_example)
