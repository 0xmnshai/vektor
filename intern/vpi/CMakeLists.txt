file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc" "intern/*.mm")

add_library(vpi STATIC ${SRC})
set_target_properties(vpi PROPERTIES AUTOMOC ON)

target_include_directories(vpi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(vpi PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(vpi PUBLIC ${CMAKE_BINARY_DIR}/generated)
target_link_libraries(vpi PUBLIC Qt6::Core Qt6::Widgets Qt6::OpenGLWidgets clog vektor_qt)

if(APPLE)
  target_link_libraries(vpi PUBLIC 
    "-framework Metal"
    "-framework QuartzCore"
    "-framework AppKit"
  )
endif()