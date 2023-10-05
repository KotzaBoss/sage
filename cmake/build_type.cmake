section_start("Build")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

section_pass("${CMAKE_BUILD_TYPE}")
