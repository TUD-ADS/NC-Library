#####################################################
## Examples
#####################################################

add_executable(reconf_ctrl controller/top_reconf_ctrl.cpp)
target_include_directories(reconf_ctrl PRIVATE . reconf_ctrl)
target_link_libraries(reconf_ctrl SystemC::systemc NC_LIB)

add_executable(EX_nested_test_rtl nested_test/top_nested_test.cpp nested_test/test_modules.cpp)
target_include_directories(EX_nested_test_rtl PRIVATE . EX_nested_test_rtl)
target_link_libraries(EX_nested_test_rtl SystemC::systemc NC_LIB)
