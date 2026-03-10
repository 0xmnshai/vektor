
file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(clog STATIC ${SRC})

target_include_directories(clog PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(clog PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(clog PRIVATE "${PROJECT_BINARY_DIR}/generated")

target_compile_definitions(clog PRIVATE VEKTOR_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

target_link_libraries(clog PUBLIC gaurdalloc)
