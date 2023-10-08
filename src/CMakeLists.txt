section_start("src")

add_library(std std.cpp)
target_precompile_headers(std PRIVATE "std.hpp")
cmake_print_properties(TARGETS std PROPERTIES SOURCES LINK_LIBRARIES)

add_library(repr STATIC repr.hpp)
target_link_libraries(repr PRIVATE fmt::fmt)
cmake_print_properties(TARGETS repr PROPERTIES SOURCES LINK_LIBRARIES)

add_library(log STATIC log.hpp)
target_link_libraries(log PRIVATE spdlog::spdlog)
cmake_print_properties(TARGETS log PROPERTIES SOURCES LINK_LIBRARIES)

add_library(layer_imgui STATIC)
target_sources(layer_imgui PRIVATE
		${imgui_SOURCE_DIR}/imgui.cpp
		${imgui_SOURCE_DIR}/imgui_draw.cpp
		${imgui_SOURCE_DIR}/imgui_tables.cpp
		${imgui_SOURCE_DIR}/imgui_widgets.cpp
		${imgui_SOURCE_DIR}/imgui_demo.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
		${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
	)
target_include_directories(layer_imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
cmake_print_properties(TARGETS layer_imgui PROPERTIES SOURCES LINK_LIBRARIES)

add_subdirectory(platform)

section_pass()
