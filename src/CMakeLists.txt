add_library(std std.cpp)
target_precompile_headers(std
	PUBLIC
		"std.hpp"
	)

set(SRC
	app.cpp
	log.cpp
	)
foreach (src ${SRC})
	string(REPLACE ".cpp" "" lib ${src})
	add_library(${lib} SHARED ${src})
	target_link_libraries(${lib} PRIVATE spdlog::spdlog)
	target_precompile_headers(${lib} REUSE_FROM std)
endforeach()

add_subdirectory("platform")
