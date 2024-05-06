cmake_minimum_required(VERSION 3.8)
project(NC_LIB)

# SystemC supports C++17
set(CMAKE_CXX_STANDARD 17)

# determine verboseness
set(VERBOSE true)

# the project source directory is initially the project root directory
set(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})
# our source directory is ./src
set(PROJECT_SOURCE_DIR src)

# include file where custom paths to systemc and boost library can be defined
set(PATHS_CONFIG_FILE "${PROJECT_ROOT_DIR}/CMakePaths.config")
if (EXISTS ${PATHS_CONFIG_FILE})
    include(${PATHS_CONFIG_FILE})
endif()

## Default SystemC path ##
if (NOT DEFINED SYSTEMC_ROOT)
    if (WIN32)
        set(SYSTEMC_ROOT "C:\\Program Files\\systemc")
    else()
        set(SYSTEMC_ROOT "/usr/local")
    endif()
endif ()

list(APPEND CMAKE_PREFIX_PATH ${SYSTEMC_ROOT})

## add SystemC package ##
find_package(SystemCLanguage CONFIG REQUIRED)
set (CMAKE_CXX_STANDARD ${SystemC_CXX_STANDARD} CACHE STRING
        "C++ standard to build all targets. Supported values are 17.")
set (CMAKE_CXX_STANDARD_REQUIRED ${SystemC_CXX_STANDARD_REQUIRED} CACHE BOOL
        "The with CMAKE_CXX_STANDARD selected C++ standard is a requirement.")

# source root as only include directory
include_directories(${PROJECT_SOURCE_DIR})

# Find the X11 package for image display (for Unix/Linux systems)
find_package(X11 REQUIRED)

#####################################################
## Library
#####################################################

add_library(${PROJECT_NAME} STATIC
    ${PROJECT_SOURCE_DIR}/region/abstract_reconf_region_base.cpp
    ${PROJECT_SOURCE_DIR}/modules/split_manager_base.cpp
    ${PROJECT_SOURCE_DIR}/modules/abstract_module_base.cpp
    ${PROJECT_SOURCE_DIR}/processes/nc_sensitive_opts.cpp
    ${PROJECT_SOURCE_DIR}/forwarding/tlm_adapter.cpp
)

# Optionally, include any additional header files
#target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/region)

target_link_libraries(${PROJECT_NAME} SystemC::systemc)

add_subdirectory(test)
add_subdirectory(example)
