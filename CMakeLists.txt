cmake_minimum_required(VERSION 3.20)
include(FetchContent)

# Do not build this in source since resources have to be copied properly
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Do not build in-source. Please remove CMakeCache.txt and the CMakeFiles/ directory. Then build out-of-source.")
endif()

# Project name and version
project(aoc VERSION 2024 DESCRIPTION "AdventOfCode Solutions by somrlik" LANGUAGES CXX)

# Get sources
set(AOC_SOURCES
        src/storage.cpp
        src/download.cpp
        src/solutions.cpp
        src/trim.cpp
        src/solutions/string_split.cpp
        src/solutions/grid.cpp
        src/solutions/Graph.cpp
        src/solutions/2022/01.cpp src/solutions/2022/02.cpp src/solutions/2022/03.cpp src/solutions/2022/04.cpp src/solutions/2022/05.cpp src/solutions/2022/06.cpp src/solutions/2022/07.cpp src/solutions/2022/08.cpp src/solutions/2022/09.cpp)
file(GLOB_RECURSE AOC_SOLUTIONS CONFIGURE_DEPENDS src/solutions/*.cpp src/solutions/*.hpp)

# Make the cli
## Use cxxopts for cli parsing
set(CXX_OPTS_TAG_VERSION v3.2.1)
file(
        DOWNLOAD
        https://raw.githubusercontent.com/jarro2783/cxxopts/refs/tags/${CXX_OPTS_TAG_VERSION}/include/cxxopts.hpp
        ${CMAKE_SOURCE_DIR}/extern/cxxopts.hpp
)

# {fmt}
set(FMT_VERSION 10.2.1)
FetchContent_Declare(fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG ${FMT_VERSION}
        )
FetchContent_MakeAvailable(fmt)

# fmtlog
set(FMTLOG_VERSION main)
file(
        DOWNLOAD
        https://github.com/MengRao/fmtlog/raw/${FMTLOG_VERSION}/fmtlog.h
        ${CMAKE_SOURCE_DIR}/extern/fmtlog/fmtlog.h
)
file(
        DOWNLOAD
        https://github.com/MengRao/fmtlog/raw/${FMTLOG_VERSION}/fmtlog-inl.h
        ${CMAKE_SOURCE_DIR}/extern/fmtlog/fmtlog-inl.h
)

# Add cli executable
add_executable(aoc
        src/cli.cpp
        ${AOC_SOURCES}
        ${AOC_SOLUTIONS}
)
target_include_directories(aoc PRIVATE
    ${CMAKE_SOURCE_DIR}/extern/fmtlog
)
target_link_libraries(aoc PRIVATE
    fmt::fmt
)
target_compile_features(aoc PRIVATE cxx_std_20)
install(TARGETS aoc
        RUNTIME DESTINATION bin
)

# curl is used to get input
option(USE_CURL "Compile with curl support to download inputs" ON)
if (USE_CURL)
    find_package(CURL REQUIRED)
    target_link_libraries(aoc PRIVATE CURL::libcurl)
    target_compile_definitions(aoc PRIVATE -DUSE_CURL)
endif()

# OpenSSL crypto was needed for a crypto solution
option(USE_OPENSSL "Compile with openssl support for solutions involving crypto" ON)
if (USE_OPENSSL)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(aoc PRIVATE OpenSSL::Crypto)
    target_compile_definitions(aoc PRIVATE -DUSE_OPENSSL)
endif()

set(CMAKE_INSTALL_DO_STRIP TRUE)

# Use googletest for testing
enable_testing()

# Testing using googletest
set(INSTALL_GTEST OFF)
set(GOOGLETEST_HEAD 03597a01ee50ed33e9dfd640b249b4be3799d395)
FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP OFF
        URL https://github.com/google/googletest/archive/${GOOGLETEST_HEAD}.zip
)
FetchContent_MakeAvailable(googletest)

# Get all the tests and link the test executable
set(TEST_EXECUTABLE_TARGET aoc-test)
set(TEST_SOURCES
        src/trim.cpp
        src/solutions/string_split.cpp
        src/solutions/grid.cpp
        src/solutions/Graph.cpp
        )
file(GLOB_RECURSE TEST_FILES CONFIGURE_DEPENDS test/*.cpp test/*.hpp)

add_executable(${TEST_EXECUTABLE_TARGET}
    ${TEST_FILES}
)
target_compile_features(${TEST_EXECUTABLE_TARGET} PRIVATE cxx_std_20)
target_sources(${TEST_EXECUTABLE_TARGET} PRIVATE ${TEST_FILES} ${TEST_SOURCES})

target_link_libraries(
        ${TEST_EXECUTABLE_TARGET}
        GTest::gtest_main
        fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(${TEST_EXECUTABLE_TARGET})

# Copy test resources
add_custom_command(
        TARGET ${TEST_EXECUTABLE_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/test/resources $<TARGET_FILE_DIR:${TEST_EXECUTABLE_TARGET}>/resources
        DEPENDS ${TEST_EXECUTABLE_TARGET}
)
