file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_editor STATIC ${SRC_FILES})

target_link_libraries(vektor_editor PUBLIC Python3::Python pybind11::embed)
target_include_directories(vektor_editor PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
