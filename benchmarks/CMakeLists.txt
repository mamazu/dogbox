add_executable(benchmarks benchmarks.cpp)
target_link_libraries(benchmarks trees in_memory_fuse_frontend_shared benchmark::benchmark)
