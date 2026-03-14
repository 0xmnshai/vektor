file(GLOB SRC "*.h" "*.hh" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(gpu STATIC ${SRC})

target_include_directories(gpu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(gpu PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(gpu PUBLIC ${CMAKE_BINARY_DIR}/generated)

target_link_libraries(gpu PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog slang)

# Subdirectories for backend implementations
# add_subdirectory(opengl)
# add_subdirectory(metal)
# add_subdirectory(intern)
