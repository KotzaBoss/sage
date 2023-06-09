cmake_minimum_required(VERSION 3.26)

project(SAGE
	VERSION 0.0.1
	DESCRIPTION "Super Advanced Game Engine"
	LANGUAGES C CXX
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

option(CCACHE_ENABLED "Use ccache to speedup subsequent compilations" OFF)
if (${CCACHE_ENABLED})
	if (NOT ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		message(WARNING "Consider not using ccache for builds other than Debug")
	endif()

	find_program(CCACHE ccache)
	if (NOT ${CCACHE} MATCHES ".*NOTFOUND")
		set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE})
		set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE})
		section_pass(${CCACHE})
	else()
		set(CCACHE_ENABLED OFF)
		section_fail("Not found")
	endif()
else()
	section_pass("disabled")
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

############################################ Compilers #
section_start("Compilers")

# Assertions
if (NOT DEFINED CMAKE_C_COMPILER)
	message(FATAL_ERROR "C compiler not defined")
elseif(NOT DEFINED CMAKE_CXX_COMPILER)
	message(FATAL_ERROR "CXX compiler not defined")
endif()

# Expectations
set(EXPECTED_COMPILER_ID "GNU")
set(EXPECTED_COMPILER_VERSION "12.0.0")
set(EXPECTED_COMPILER_DEBUG "${EXPECTED_COMPILER_ID} ${EXPECTED_COMPILER_VERSION}") 

# Checks
list(APPEND langs	C				CXX)
list(APPEND compilers	${CMAKE_C_COMPILER}		${CMAKE_CXX_COMPILER})
list(APPEND ids		${CMAKE_C_COMPILER_ID}		${CMAKE_CXX_COMPILER_ID})
list(APPEND versions	${CMAKE_C_COMPILER_VERSION}	${CMAKE_CXX_COMPILER_VERSION})
foreach (lang compiler id version IN ZIP_LISTS langs compilers ids versions)
	section_next("${lang}")
	message(CHECK_START "${compiler} ${id} ${version}")
	if (NOT ${id} STREQUAL ${EXPECTED_COMPILER_ID})
		message(FATAL_ERROR "Expected compiler: ${EXPECTED_COMPILER_ID} found: ${id}")
	elseif(${version} VERSION_LESS ${EXPECTED_COMPILER_VERSION})
		message(FATAL_ERROR "Expected compiler version: ${EXPECTED_COMPILER_VERSION} found: ${version}")
	else()
		message(CHECK_PASS "Ok")
	endif()
endforeach()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS TRUE)

section_pass("Ok")

############################################ Subdirectories #
section_start("Subdirectories")

section_next("src")
include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

section_next("test")
add_subdirectory(test)

section_pass("Ok")

############################################ Summary #

message(STATUS "==================================================================")
message(STATUS "	${PROJECT_NAME} ${PROJECT_VERSION}")
message(STATUS "	${PROJECT_DESCRIPTION}")
message(STATUS "..................................................................")
message(STATUS "	${CMAKE_BUILD_TYPE}")
message(STATUS "	${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_COMPILER_VERSION}")
# TODO: Can we use generator expression here somehow?
if (${CCACHE_ENABLED})
	message(STATUS "	ccache enabled")
else()
	message(STATUS "	ccache disabled")
endif()
message(STATUS "==================================================================")

