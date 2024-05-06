#####################################################
## Tests
#####################################################

project(general_test)
add_executable(${PROJECT_NAME} pure_tlm_interface/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(reconf_during_transaction)
add_executable(${PROJECT_NAME} pure_tlm_interface/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(transaction_during_reconf)
add_executable(${PROJECT_NAME} pure_tlm_interface/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(ports_and_sockets_test)
add_executable(${PROJECT_NAME} port_tests/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(double_write)
add_executable(${PROJECT_NAME} write_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(thread_test)
add_executable(${PROJECT_NAME} thread_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(cthread_test)
add_executable(${PROJECT_NAME} cthread_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(method_test)
add_executable(${PROJECT_NAME} method_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(splitted_test)
add_executable(${PROJECT_NAME} splitted_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(nested_test)
add_executable(${PROJECT_NAME} nested_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(nested_unload_test)
add_executable(${PROJECT_NAME} nested_unload/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(immediate_transaction_test)
add_executable(${PROJECT_NAME} immediate_transaction_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(inner_signal_test)
add_executable(${PROJECT_NAME} inner_signal_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(inner_signal_many_writers_test)
add_executable(${PROJECT_NAME} inner_signal_many_writers_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(preload_test)
add_executable(${PROJECT_NAME} preload_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

project(vector_test)
add_executable(${PROJECT_NAME} vector_test/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
list(APPEND all_test_targets ${PROJECT_NAME})

add_custom_target(build_all_tests)
foreach(target IN LISTS all_test_targets)
    add_dependencies(build_all_tests ${target})
endforeach()

list(JOIN all_test_targets " " test_targets_list)
configure_file(run_all_tests.cpp.tmpl run_all_tests.cpp)

project(_run_all_tests)
add_executable(_run_all_tests run_all_tests.cpp)
add_dependencies(_run_all_tests build_all_tests)

add_subdirectory(readme_example)
