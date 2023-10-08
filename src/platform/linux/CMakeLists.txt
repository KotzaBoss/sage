section_start("linux")

add_library(linux_window STATIC window.hpp)
add_dependencies(linux_window glad)
target_link_libraries(linux_window PRIVATE glad glfw GL stb)
target_include_directories(linux_window PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})
cmake_print_properties(TARGETS linux_window PROPERTIES SOURCES LINK_DIRECTORIES)

section_pass()
