file(GLOB sources *.h *.cpp)
add_library(fuse_adaptor ${sources})
target_link_libraries(fuse_adaptor trees ${FUSE_LIB})
