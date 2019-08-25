file(GLOB sources *.h *.cpp ../common/*)
add_executable(tests ${sources})
target_link_libraries(tests in_memory_fuse_frontend_shared Boost::unit_test_framework)
