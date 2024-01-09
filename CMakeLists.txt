cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP_HELPERS C CXX)

add_library(clap-helpers INTERFACE)
target_include_directories(clap-helpers SYSTEM INTERFACE include)
target_link_libraries(clap-helpers INTERFACE clap)

# CLAP_BUILD_TESTS is inherited from clap
if (${CLAP_BUILD_TESTS})
    enable_testing()
    find_package(Catch2 3 QUIET)

    if (Catch2_FOUND)
        add_executable(clap-helpers-tests EXCLUDE_FROM_ALL
            tests/hex-encoder.cc
            tests/plugin.cc
            tests/param-queue-tests.cc
            tests/event-list-tests.cc
            tests/main.cc
            tests/preset-discovery-indexer.cc
            tests/preset-discovery-provider.cc
            tests/preset-discovery-metadata-receiver.cc)
        set_target_properties(clap-helpers-tests PROPERTIES CXX_STANDARD 11)
        target_link_libraries(clap-helpers-tests clap-helpers clap Catch2::Catch2WithMain)
        target_compile_definitions(clap-helpers-tests PUBLIC -DCATCH_CONFIG_PREFIX_ALL)
        add_test(NAME test-clap-helpers COMMAND clap-helpers-tests)
        add_dependencies(clap-tests clap-helpers-tests)
    else()
        message(STATUS "Catch2 >= 3 isn't found -> disable clap-helpers unit tests")
    endif()
endif()

install(DIRECTORY include DESTINATION ".")
install(FILES "cmake/clap-helpers-config.cmake" DESTINATION "./lib/cmake/clap-helpers")
