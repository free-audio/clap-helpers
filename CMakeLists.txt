cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP_PLUGINS C CXX)

add_subdirectory(clap)
add_subdirectory(glue)
add_subdirectory(plugins)
