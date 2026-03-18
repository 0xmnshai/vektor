file(GLOB SRC "*.h" "*.hh" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_menu_edit STATIC ${SRC})

set_target_properties(vektor_qt_menu_edit PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_menu_edit PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vektor_qt_menu_edit PUBLIC 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets 
    clog 
    ecs
)
