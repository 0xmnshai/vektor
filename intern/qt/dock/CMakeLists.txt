file(GLOB SRC "*.h" "*.cc" "intern/*.hh" "intern/*.cc")

add_library(vektor_qt_dock STATIC ${SRC})

set_target_properties(vektor_qt_dock PROPERTIES AUTOMOC ON)

target_include_directories(vektor_qt_dock PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# If common headers are elsewhere, add them here or via main target