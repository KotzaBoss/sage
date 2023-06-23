message(STATUS "std")
add_library(std std.cpp)
target_precompile_headers(std PUBLIC "std.hpp")

message(STATUS "log")
add_library(log STATIC log.cpp)
message(STATUS "L spdlog")
target_link_libraries(log PRIVATE spdlog::spdlog)

file(GLOB SRC
	*.cpp
	)
list(FILTER SRC EXCLUDE REGEX ".*/(std|log).cpp")
foreach (src ${SRC})
	cmake_path(GET src FILENAME src)
	string(REPLACE ".cpp" "" lib ${src})
	message(STATUS "${lib}")

	add_library(${lib} SHARED ${src})

	message(STATUS "L log")
	target_link_libraries(${lib}
		PRIVATE
			log
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
