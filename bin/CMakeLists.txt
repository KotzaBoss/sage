section_start("bin")

add_executable(sage main.cpp)
add_dependencies(sage shaders textures)
target_include_directories(sage PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(sage PRIVATE doctest repr log layer_imgui linux_window event)
target_precompile_headers(sage REUSE_FROM std)

cmake_print_properties(TARGETS sage PROPERTIES SOURCES LINK_LIBRARIES)

section_pass()
