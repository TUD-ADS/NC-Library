#####################################################
## Examples
#####################################################

project(reconf_ctrl)
add_executable(${PROJECT_NAME} controller/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)

project(EX_nested_test_rtl)
add_executable(${PROJECT_NAME} nested_test/top_nested_test.cpp nested_test/test_modules.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE . ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
