add_executable(fuse_frontend main.cpp)
target_link_libraries(fuse_frontend shared_stuff ${FUSE_LIB})
