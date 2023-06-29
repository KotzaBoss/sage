message(STATUS "sandbox")
add_executable(sandbox main.cpp)

target_include_directories(sandbox
	PRIVATE
		${PROJECT_SOURCE_DIR}/src
	)

target_link_libraries(sandbox
	PRIVATE
		doctest
		log
		repr
		layer_imgui
		linux_window
		event
	)

target_precompile_headers(sandbox REUSE_FROM std)

