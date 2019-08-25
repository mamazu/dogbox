file(GLOB sources *.h *.cpp)
add_library(trees ${sources})
target_link_libraries(trees blob_layer pthread)
