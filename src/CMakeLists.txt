add_library(std std.cpp)
target_precompile_headers(std
	PUBLIC
		"std.hpp"
	)

file(GLOB SRC
	*.cpp
	)
list(FILTER SRC EXCLUDE REGEX ".*/std.cpp") 
foreach (src ${SRC})
	cmake_path(GET src FILENAME src)
	string(REPLACE ".cpp" "" lib ${src})

	if (${lib} STREQUAL "log")
		add_library(${lib} STATIC ${src})
		target_link_libraries(${lib} PRIVATE spdlog::spdlog)
	else()
		add_library(${lib} SHARED ${src})
	endif()
	
	target_precompile_headers(${lib} REUSE_FROM std)
endforeach()

add_subdirectory("platform")
