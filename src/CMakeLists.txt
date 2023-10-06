section_start("src")

add_library(std std.cpp)
target_precompile_headers(std PRIVATE "std.hpp")
cmake_print_properties(TARGETS std PROPERTIES SOURCES LINK_LIBRARIES)

add_library(log STATIC log.cpp)
target_include_directories(log PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(log PRIVATE spdlog::spdlog linux_window)
cmake_print_properties(TARGETS log PROPERTIES SOURCES LINK_LIBRARIES)

add_library(repr STATIC repr.cpp)
target_include_directories(repr PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(repr PRIVATE fmt::fmt linux_window)
cmake_print_properties(TARGETS repr PROPERTIES SOURCES LINK_LIBRARIES)

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

file(GLOB SRC *.cpp)
list(FILTER SRC EXCLUDE REGEX ".*/(std|log|repr).cpp")
foreach (src ${SRC})
	cmake_path(GET src FILENAME src)
	string(REPLACE ".cpp" "" lib ${src})

	add_library(${lib} STATIC ${src})
	target_include_directories(${lib} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
	target_link_libraries(${lib} PRIVATE log repr)

	if (${lib} STREQUAL "window")
		target_link_libraries(${lib} PUBLIC event)
	endif()

	target_precompile_headers(${lib} REUSE_FROM std)

	cmake_print_properties(TARGETS ${lib} PROPERTIES SOURCES LINK_LIBRARIES)
endforeach()

add_subdirectory(platform)

section_pass("Ok")
