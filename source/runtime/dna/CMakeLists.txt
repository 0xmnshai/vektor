file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(dna STATIC ${SRC})
set_target_properties(dna PROPERTIES AUTOMOC ON)

target_include_directories(dna PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(dna PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(dna PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(dna PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog)