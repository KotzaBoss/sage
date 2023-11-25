section_start("bin")

add_executable(sage main.cpp)
add_dependencies(sage shaders textures)
target_include_directories(sage PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(sage PRIVATE doctest repr log layer_imgui linux_window event)
target_precompile_headers(sage REUSE_FROM std)

cmake_print_properties(TARGETS sage PROPERTIES SOURCES LINK_LIBRARIES)

docs(SET TARGET sage DOCS "    cmake --build build -- sage && ${CMAKE_CURRENT_BINARY_DIR}/sage")

section_pass()
