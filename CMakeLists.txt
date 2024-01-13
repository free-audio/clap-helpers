cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(CLAP_HELPERS C CXX)

set(add_tests ${CLAP_BUILD_TESTS})
set(add_tests_to_clap_tests TRUE)
set(skip_install FALSE)
set(tests_cxx_standard 11)
set(download_packages FALSE)
# define special handling for CLAP_HELPERS_TESTS_ONLY:
if (${CLAP_HELPERS_TESTS_ONLY})
    set(add_tests TRUE)
    set(add_tests_to_clap_tests FALSE)
    set(skip_install TRUE)
    if (DEFINED CLAP_HELPERS_TESTS_CXX_STANDARD)
        set(tests_cxx_standard ${CLAP_HELPERS_TESTS_CXX_STANDARD})
    else()
        set(tests_cxx_standard 17)
    endif()
    message(STATUS "clap-helpers-tests: including CPM")
    include(cmake/external/CPM.cmake)
    set(download_packages TRUE)
endif()

add_library(clap-helpers INTERFACE)
target_include_directories(clap-helpers SYSTEM INTERFACE include)
if (${download_packages})
    CPMAddPackage(
        NAME "clap"
        GITHUB_REPOSITORY "free-audio/clap"
        GIT_TAG "1.1.10"
        EXCLUDE_FROM_ALL TRUE
        DOWNLOAD_ONLY TRUE
        SOURCE_DIR cpm/clap
    )
    find_package(clap QUIET)
else()
    target_link_libraries(clap-helpers INTERFACE clap)
endif()

if (${add_tests})
    if(${add_tests_to_clap_tests})
        enable_testing()
    endif()
    if (${download_packages})
        CPMAddPackage(
            NAME "Catch2"
            GITHUB_REPOSITORY "catchorg/Catch2"
            GIT_TAG "v3.5.1"
            EXCLUDE_FROM_ALL TRUE
            DOWNLOAD_ONLY TRUE
            SOURCE_DIR cpm/catch2
        )
    endif()
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
            tests/preset-discovery-metadata-receiver.cc
        )
        set_target_properties(clap-helpers-tests PROPERTIES CXX_STANDARD ${tests_cxx_standard})
        target_include_directories(clap-helpers-tests SYSTEM INTERFACE include)
        target_link_libraries(clap-helpers-tests clap-helpers clap Catch2::Catch2WithMain)
        target_compile_definitions(clap-helpers-tests PUBLIC -DCATCH_CONFIG_PREFIX_ALL)
        if(${add_tests_to_clap_tests})
            add_test(NAME test-clap-helpers COMMAND clap-helpers-tests)
            add_dependencies(clap-helpers clap-helpers-tests)
        endif()
    else()
        message(STATUS "clap-helpers: Catch2 >= 3 isn't found -> disable unit tests")
    endif()
endif()

if (NOT ${skip_install})
    install(DIRECTORY include DESTINATION ".")
    install(FILES "cmake/clap-helpers-config.cmake" DESTINATION "./lib/cmake/clap-helpers")
endif()
