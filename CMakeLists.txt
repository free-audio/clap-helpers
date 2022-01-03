cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP_HELPERS C CXX)

add_library(clap-helpers INTERFACE)
set_target_properties(clap-helpers PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    CXX_STANDARD 11)
target_include_directories(clap-helpers INTERFACE include)
target_link_libraries(clap-helpers INTERFACE clap-core)

add_executable(clap-helpers-cxx11-compilation-test EXCLUDE_FROM_ALL tests/c++11-compile.cc)
set_target_properties(clap-helpers-cxx11-compilation-test PROPERTIES CXX_STANDARD 11)
target_link_libraries(clap-helpers-cxx11-compilation-test clap-helpers clap-core)

install(DIRECTORY include DESTINATION ".")
