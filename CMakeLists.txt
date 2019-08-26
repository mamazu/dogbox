cmake_minimum_required(VERSION 3.15)
project(dogbox LANGUAGES CXX)

set(DEPENDENCIES_INSTALL_DIR ${CMAKE_SOURCE_DIR}/../dogbox_dependencies/install)
set(CMAKE_PREFIX_PATH ${DEPENDENCIES_INSTALL_DIR})

find_package(SQLite3 REQUIRED)

set(BOOST_ROOT ${DEPENDENCIES_INSTALL_DIR})
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost COMPONENTS system unit_test_framework filesystem REQUIRED)

set(benchmark_ROOT_DIR ${DEPENDENCIES_INSTALL_DIR})
find_package(benchmark REQUIRED)

find_library(FUSE_LIB fuse)

find_package(OpenSSL REQUIRED)

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
    -Wuseless-cast
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
    -DFUSE_USE_VERSION=26
)

include_directories(.)

add_subdirectory(blob_layer)
add_subdirectory(trees)
add_subdirectory(fuse_adaptor)
add_subdirectory(in_memory_fuse_frontend_shared)
add_subdirectory(benchmarks)
add_subdirectory(in_memory_fuse_frontend)
add_subdirectory(tests)
