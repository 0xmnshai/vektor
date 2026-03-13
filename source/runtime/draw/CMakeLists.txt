file(GLOB SRC "*.h" "*.hh" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(draw STATIC ${SRC})

target_include_directories(draw PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(draw PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(draw PUBLIC gpu dna)
