cmake_minimum_required(VERSION 3.9)

project(webclient VERSION 1.0.0 DESCRIPTION "https client")


# Boost
# https://cmake.org/cmake/help/v3.8/module/FindBoost.html
set (Boost_NO_SYSTEM_PATHS       ON)
set (Boost_USE_MULTITHREADED     ON)
set (Boost_USE_STATIC_RUNTIME    OFF)

find_package (Boost REQUIRED system filesystem)

link_directories (
    ${Boost_LIBRARY_DIRS})

include_directories (
    ${Boost_INCLUDE_DIRS}
    $ENV{PROJECT_SRC_DIR})

add_definitions(-DBOOST_LOG_DYN_LINK)

# other libs here

# compiler flags
set (CMAKE_CXX_FLAGS                "-Wall -ggdb -std=c++17")

# set (GCC_COVERAGE_COMPILE_FLAGS   "-fprofile-arcs -ftest-coverage")
# set (GCC_COVERAGE_LINK_FLAGS      "-lgcov")


# Target
add_subdirectory(src)
