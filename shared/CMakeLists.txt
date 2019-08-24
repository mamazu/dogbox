file(GLOB sources *.h *.cpp)
add_library(shared_stuff ${sources})
target_include_directories(shared_stuff PUBLIC .)
