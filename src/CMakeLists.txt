message(STATUS "std")
add_library(std std.cpp)
target_precompile_headers(std PUBLIC "std.hpp")

message(STATUS "log")
add_library(log STATIC log.cpp)
message(STATUS "L spdlog")
target_link_libraries(log
	PRIVATE
		spdlog::spdlog
		linux_window
	)

message(STATUS "repr")
add_library(repr STATIC repr.cpp)
target_link_libraries(repr
	PRIVATE
		fmt::fmt
		linux_window
	)

message(STATUS "layer_imgui")
set(layer_imgui_src
	${PROJECT_SOURCE_DIR}/imgui/imgui.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_draw.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_tables.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_widgets.cpp
	${PROJECT_SOURCE_DIR}/imgui/imgui_demo.cpp
	${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp
	${PROJECT_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp
	)
foreach(src ${layer_imgui_src})
	message(STATUS "SRC ${src}")
endforeach()
add_library(layer_imgui STATIC ${layer_imgui_src})

file(GLOB SRC
	*.cpp
	)
list(FILTER SRC EXCLUDE REGEX ".*/(std|log|repr).cpp")
foreach (src ${SRC})
	cmake_path(GET src FILENAME src)
	string(REPLACE ".cpp" "" lib ${src})
	message(STATUS "${lib}")

	add_library(${lib} STATIC ${src})

	message(STATUS "L log")
	message(STATUS "L repr")
	target_link_libraries(${lib}
		PRIVATE
			log
			repr
		)

	if (${lib} STREQUAL "window")
		message(STATUS "L event")
		target_link_libraries(${lib}
			PUBLIC
				event
			)
	endif()

	target_precompile_headers(${lib} REUSE_FROM std)
endforeach()

add_subdirectory("platform")
