file(GLOB SRC "*.h" "*.hh" "*.cc" "*.mm" "intern/*.hh" "intern/*.cc" "intern/*.mm")

add_library(shaders STATIC ${SRC})

target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(shaders PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(shaders PUBLIC ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(shaders PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog slang)