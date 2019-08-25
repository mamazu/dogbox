file(GLOB sources *.h *.cpp)
add_executable(tests ${sources})
target_link_libraries(tests shared_stuff Boost::unit_test_framework)
