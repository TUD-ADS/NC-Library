project(readme_example)
add_executable(${PROJECT_NAME} src/top_${PROJECT_NAME}.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE src ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} SystemC::systemc NC_LIB)
