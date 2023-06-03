cmake_minimum_required(VERSION 3.26)

project(SAGE
	VERSION 0.0.1
	DESCRIPTION "Super Advanced Game Engine"
	LANGUAGES CXX
)

############################################ Build Type #

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

message(STATUS "Configuring ${CMAKE_BUILD_TYPE} build")

############################################ Compiler #

set(EXPECTED_CXX_COMPILER "g++")
set(EXPECTED_CXX_COMPILER_ID "GNU")
set(EXPECTED_CXX_COMPILER_VERSION "12.0.0")
set(EXPECTED_CXX_COMPILER_DEBUG "${EXPECTED_CXX_COMPILER_ID} ${EXPECTED_CXX_COMPILER} ${EXPECTED_CXX_COMPILER_VERSION}") 

message(CHECK_START "Confirming compiler as: ${EXPECTED_CXX_COMPILER_DEBUG}")
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL ${EXPECTED_CXX_COMPILER_ID})
	message(FATAL_ERROR "Expected compiler: ${EXPECTED_CXX_COMPILER_ID} found: ${CMAKE_CXX_COMPILER_ID}")
elseif(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS ${EXPECTED_CXX_COMPILER_VERSION})
	message(FATAL_ERROR "Expected compiler version: ${EXPECTED_CXX_COMPILER_VERSION} found: ${CMAKE_CXX_COMPILER_VERSION}")
endif()
message(CHECK_PASS "Ok")

############################################ ccache #

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	message(CHECK_START "Finding ccache for Debug build")
	find_program(CCACHE ccache)
	if (NOT ${CCACHE} MATCHES ".*NOTFOUND")
		message(CHECK_PASS ${CCACHE})
		set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
		set(CCACHE_ENABLED TRUE)
	else()
		message(CHECK_FAIL "Not found")
		set(CCACHE_ENABLED FALSE)
	endif()
endif()

############################################ Requirements #

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

include(CTest)

############################################ Subdirectories #

add_subdirectory(src)
add_subdirectory(test)

message("==================================================================")
message("	${PROJECT_NAME} ${PROJECT_VERSION}")
message("	${PROJECT_DESCRIPTION}")
message("..................................................................")
message("	${CMAKE_BUILD_TYPE}")
message("	${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
# TODO: Can we use generator expression here somehow?
if (${CCACHE_ENABLED})
	message("	ccache enabled")
else()
	message("	ccache disabled")
endif()
message("==================================================================")
