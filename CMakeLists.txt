cmake_minimum_required(VERSION 3.17)
cmake_policy(SET CMP0100 NEW)  # handle .hh files
project(clap-helpers C CXX)

option(CLAP_HELPERS_BUILD_TESTS "Build tests for the CLAP Helper" FALSE)
option(CLAP_HELPERS_DOWNLOAD_DEPENDENCIES "Resolve CLAP Targets with CPM if not defined" FALSE)
option(CLAP_HELPERS_DOWNLOAD_CLAP_BRANCH_IS_NEXT "If downloading, download against against 'next' branch" FALSE)
option(CLAP_HELPERS_NO_CLAP_IS_FATAL "If there is no clap target and no download, emit a fatal error" TRUE)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE include)

if (NOT TARGET clap)
    if (${CLAP_HELPERS_DOWNLOAD_DEPENDENCIES})
        if (${CLAP_HELPERS_DOWNLOAD_CLAP_BRANCH_IS_NEXT})
           set(cpmbranch "next")
        else()
           set(cpmbranch "main")
        endif()
        message(STATUS "${PROJECT_NAME}: Downloading CLAP on branch ${cpmbranch} using CPM")

        message(STATUS "${PROJECT_NAME}: including CPM")
        include(cmake/external/CPM.cmake)
        CPMAddPackage(
                NAME "clap"
                GITHUB_REPOSITORY "free-audio/clap"
                GIT_TAG ${cpmbranch}
                EXCLUDE_FROM_ALL TRUE
                DOWNLOAD_ONLY TRUE
                SOURCE_DIR cpm/clap
        )

        add_subdirectory(${CMAKE_BINARY_DIR}/cpm/clap)
    else()
        if (${CLAP_HELPERS_NO_CLAP_IS_FATAL})
            message(FATAL_ERROR "${PROJECT_NAME}: 'clap' is not a target. Set CLAP_HELPERS_DOWNLOAD_DEPENDENCIES or include clap before clap-helpers")
        else()
            message(WARNING "${PROJECT_NAME}: 'clap' is not a target. Set CLAP_HELPERS_DOWNLOAD_DEPENDENCIES or include clap before clap-helpers. Build proceeding anyway. Good luck.")
        endif()
    endif()
endif()
target_link_libraries(${PROJECT_NAME} INTERFACE clap)

if (${CLAP_HELPERS_BUILD_TESTS})
    if (NOT DEFINED CLAP_HELPERS_TESTS_CXX_STANDARD)
        message(STATUS "${PROJECT_NAME}: defaulting to c++11")
        set(CLAP_HELPERS_TESTS_CXX_STANDARD 11)
    else()
        message(STATUS "${PROJECT_NAME}: Using CXX Standard ${CLAP_HELPERS_TESTS_CXX_STANDARD}")
    endif()

    if ((NOT TARGET Catch2::Catch2WithMain) AND ${CLAP_HELPERS_DOWNLOAD_DEPENDENCIES})
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
    else()
        message(FATAL_ERROR "${PROJECT_NAME}: 'Catch2::Catch2WithMain' is not a target. Set CLAP_HELPERS_DOWNLOAD_DEPENDENCIES or include catch2 before clap-helpers")
    endif()

    add_executable(${PROJECT_NAME}-tests EXCLUDE_FROM_ALL
            tests/context-menu-builder-compiles.cc
            tests/create-an-actual-host.cc
            tests/create-an-actual-plugin.cc
            tests/hex-encoder.cc
            tests/host.cc
            tests/plugin.cc
            tests/param-queue-tests.cc
            tests/event-list-tests.cc
            tests/main.cc
            tests/preset-discovery-indexer.cc
            tests/preset-discovery-provider.cc
            tests/preset-discovery-metadata-receiver.cc
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD ${CLAP_HELPERS_TESTS_CXX_STANDARD})
    set_target_properties(${PROJECT_NAME}-tests PROPERTIES CXX_STANDARD ${CLAP_HELPERS_TESTS_CXX_STANDARD})
    target_link_libraries(${PROJECT_NAME}-tests ${PROJECT_NAME} Catch2::Catch2WithMain)
    target_compile_definitions(${PROJECT_NAME}-tests PUBLIC -DCATCH_CONFIG_PREFIX_ALL)

    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${PROJECT_NAME}-tests
           PRIVATE
           -Werror
        )
    endif()

    add_custom_command(TARGET ${PROJECT_NAME}-tests POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Relocating $<TARGET_FILE:${PROJECT_NAME}-tests> to ${CMAKE_BINARY_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}-tests>" "${CMAKE_BINARY_DIR}"
    )
endif()

install(DIRECTORY include DESTINATION ".")
install(FILES "cmake/clap-helpers-config.cmake" DESTINATION "./lib/cmake/clap-helpers")
