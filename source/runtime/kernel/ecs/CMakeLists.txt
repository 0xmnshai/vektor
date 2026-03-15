file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(ecs STATIC ${SRC})
set_target_properties(ecs PROPERTIES AUTOMOC ON)

target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(ecs PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(ecs PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(ecs PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog EnTT::EnTT)