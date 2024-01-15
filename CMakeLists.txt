cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(clap-helpers C CXX)

option(CLAP_HELPERS_BUILD_TESTS "Build tests for the CLAP Helper" FALSE)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

if (NOT TARGET clap)
    message(STATUS "${PROJECT_NAME}: Downloading CLAP using CPM")

    message(STATUS "${PROJECT_NAME}: including CPM")
    include(cmake/external/CPM.cmake)
    CPMAddPackage(
            NAME "clap"
            GITHUB_REPOSITORY "free-audio/clap"
            GIT_TAG "next"
            EXCLUDE_FROM_ALL TRUE
            DOWNLOAD_ONLY TRUE
            SOURCE_DIR cpm/clap
    )

    add_subdirectory(${CMAKE_BINARY_DIR}/cpm/clap)
endif()
target_link_libraries(${PROJECT_NAME} INTERFACE clap)

if (${CLAP_HELPERS_BUILD_TESTS})
    if (NOT DEFINED ${CLAP_HELPERS_TESTS_CXX_STANDARD})
        message(STATUS "${PROJECT_NAME}: defaualting to c++11")
        set(CLAP_HELPERS_TESTS_CXX_STANDARD 11)
    endif()

    if (NOT TARGET Catch2::Catch2WithMain)
        message(STATUS "${PROJECT_NAME}: Downloading Catch2")
        include(cmake/external/CPM.cmake)
        CPMAddPackage(
                NAME "Catch2"
                GITHUB_REPOSITORY "catchorg/Catch2"
                GIT_TAG "v3.5.1"
                EXCLUDE_FROM_ALL TRUE
                DOWNLOAD_ONLY TRUE
                SOURCE_DIR cpm/catch2
        )
        add_subdirectory(${CMAKE_BINARY_DIR}/cpm/catch2)
    endif()

    add_executable(${PROJECT_NAME}-tests EXCLUDE_FROM_ALL
            tests/hex-encoder.cc
            tests/plugin.cc
            tests/param-queue-tests.cc
            tests/event-list-tests.cc
            tests/main.cc
            tests/preset-discovery-indexer.cc
            tests/preset-discovery-provider.cc
            tests/preset-discovery-metadata-receiver.cc
    )
    set_target_properties(${PROJECT_NAME}-tests PROPERTIES CXX_STANDARD ${CLAP_HELPERS_TESTS_CXX_STANDARD})
    target_link_libraries(${PROJECT_NAME}-tests ${PROJECT_NAME} Catch2::Catch2WithMain)
    target_compile_definitions(${PROJECT_NAME}-tests PUBLIC -DCATCH_CONFIG_PREFIX_ALL)
endif()

install(DIRECTORY include DESTINATION ".")
install(FILES "cmake/clap-helpers-config.cmake" DESTINATION "./lib/cmake/clap-helpers")
