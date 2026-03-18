file(GLOB SRC "*.h" "*.hh" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vmo STATIC ${SRC})

target_include_directories(vmo PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(vmo PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(vmo PUBLIC dna glm)
