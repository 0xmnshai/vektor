file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(rna STATIC ${SRC})
set_target_properties(rna PROPERTIES AUTOMOC ON)

target_include_directories(rna PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(rna PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(rna PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(rna PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog dna EnTT::EnTT)