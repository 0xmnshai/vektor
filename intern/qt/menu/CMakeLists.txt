file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_menu STATIC ${SRC})

set_target_properties(vektor_qt_menu PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_menu PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(vektor_qt_menu PUBLIC 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets 
    clog 
    vektor_qt
    vektor_qt_menu_window
    vektor_qt_menu_file
    vektor_qt_menu_edit
)

add_subdirectory(window)
add_subdirectory(file)
add_subdirectory(edit)