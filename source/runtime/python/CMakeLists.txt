file(GLOB_RECURSE SRC_FILES *.h *.c *.cc *.cpp)

add_library(vektor_python STATIC ${SRC_FILES})
target_include_directories(vektor_python PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${Python3_INCLUDE_DIRS})
target_link_libraries(vektor_python PUBLIC Python3::Python pybind11::embed pybind11::module)
