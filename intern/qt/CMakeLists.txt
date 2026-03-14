
set(INC
  .
  ui
)

set(INC_SYS
  ${Qt6Core_INCLUDE_DIRS}
  ${Qt6Widgets_INCLUDE_DIRS}
  ${Qt6OpenGLWidgets_INCLUDE_DIRS}
)

add_library(vektor_qt INTERFACE)

# Add subdirectories
add_subdirectory(ui)
add_subdirectory(dock)
# add_subdirectory(menu)

target_link_libraries(vektor_qt 
  INTERFACE 
    Qt6::Core 
    Qt6::Widgets 
    Qt6::OpenGLWidgets
    vektor_qt_ui
    vektor_qt_dock
)
