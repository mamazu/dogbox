file(GLOB sources *.h *.cpp)
add_library(in_memory_fuse_frontend_shared ${sources})
target_link_libraries(in_memory_fuse_frontend_shared stdc++fs)
