cmake_minimum_required(VERSION 3.26)

project(SAGE
	VERSION 0.0.1
	DESCRIPTION "Super Advanced Game Engine"
	LANGUAGES CXX
)

message("==================================================================")

macro(section_start msg)
	message(CHECK_START ${msg})
	list(APPEND CMAKE_MESSAGE_INDENT "\t")
endmacro()

macro(section_next msg)
	list(POP_BACK CMAKE_MESSAGE_INDENT)
	message(STATUS ${msg})
	list(APPEND CMAKE_MESSAGE_INDENT "\t")
endmacro()

macro(section_pass msg)
	list(POP_BACK CMAKE_MESSAGE_INDENT)
	message(CHECK_PASS ${msg})
endmacro()

macro(section_fail msg)
	list(POP_BACK CMAKE_MESSAGE_INDENT)
	message(CHECK_FAIL ${msg})
endmacro()

############################################ Build Type #
section_start("Build type")

if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Debug")
endif()

section_pass("${CMAKE_BUILD_TYPE}")
############################################ ccache #
section_start("ccache")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	find_program(CCACHE ccache)
	if (NOT ${CCACHE} MATCHES ".*NOTFOUND")
		set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
		set(CCACHE_ENABLED TRUE)
		section_pass(${CCACHE})
	else()
		set(CCACHE_ENABLED FALSE)
		section_fail("Not found")
	endif()
else()
	section_pass("disabled, build is not Debug")
endif()

############################################ Packages #
section_start("Packages")

#find_package(OpenGL REQUIRED)
#find_package(GLEW REQUIRED)
#find_package(glfw3 REQUIRED)
find_package(Threads REQUIRED)

include(CTest)

section_pass("Ok")
############################################ Dependencies #
section_start("Dependencies")

section_next("spdlog")
set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
add_subdirectory(spdlog)
include_directories(${PROJECT_SOURCE_DIR}/spdlog/include)

section_next("glfw")
add_subdirectory(glfw)
include_directories(${PROJECT_SOURCE_DIR}/glfw/include)

section_pass("Ok")
############################################ Compiler #
section_start("Compiler")

set(EXPECTED_CXX_COMPILER_ID "GNU")
set(EXPECTED_CXX_COMPILER_VERSION "12.0.0")
set(EXPECTED_CXX_COMPILER_DEBUG "${EXPECTED_CXX_COMPILER_ID} ${EXPECTED_CXX_COMPILER_VERSION}") 

section_next("Expecting ${EXPECTED_CXX_COMPILER_DEBUG}")
if (NOT ${CMAKE_CXX_COMPILER_ID} STREQUAL ${EXPECTED_CXX_COMPILER_ID})
	message(FATAL_ERROR "Expected compiler: ${EXPECTED_CXX_COMPILER_ID} found: ${CMAKE_CXX_COMPILER_ID}")
elseif(${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS ${EXPECTED_CXX_COMPILER_VERSION})
	message(FATAL_ERROR "Expected compiler version: ${EXPECTED_CXX_COMPILER_VERSION} found: ${CMAKE_CXX_COMPILER_VERSION}")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS TRUE)

section_pass("Ok")
############################################ Subdirectories #
section_start("Subdirectories")

section_next("src")
add_subdirectory(src)
include_directories(${PROJECT_SOURCE_DIR}/src)

section_next("test")
add_subdirectory(test)

section_pass("Ok")
############################################ Summary #

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
