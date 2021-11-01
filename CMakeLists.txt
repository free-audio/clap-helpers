cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP_HELPERS C CXX)

add_library(clap-helpers INTERFACE)
target_include_directories(clap-helpers INTERFACE include)
target_link_libraries(clap-helpers INTERFACE clap-core)

install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_PREFIX}")
