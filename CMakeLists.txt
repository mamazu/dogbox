cmake_minimum_required(VERSION 3.15)
project(dogbox LANGUAGES CXX)

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../dogbox_dependencies/install)
set(CMAKE_PREFIX_PATH ${DEPENDENCIES_INSTALL_DIR})

find_package(SQLite3 REQUIRED)

set(BOOST_ROOT ${DEPENDENCIES_INSTALL_DIR})
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system unit_test_framework REQUIRED)

set(benchmark_ROOT_DIR ${DEPENDENCIES_INSTALL_DIR})
find_package(benchmark REQUIRED)

find_library(FUSE_LIB fuse)

add_definitions(
    -Wall
    -Wextra
    -Wduplicated-cond
    -Wduplicated-branches
    -Wlogical-op
    -Wrestrict
    -Wnull-dereference
    -Wdouble-promotion
    -Wshadow
    -fstrict-aliasing
    -Wformat=2
    -pedantic
    -Werror
    -Wconversion
    -Wunused
    -Wundef
    -Wvla
    -Wswitch-enum
    -Wuninitialized
    -Wstrict-overflow=5
    -std=c++17
    -D_FILE_OFFSET_BITS=64
)

add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks benchmark::benchmark SQLite::SQLite3)

add_subdirectory(fuse_frontend)
add_subdirectory(tests)
