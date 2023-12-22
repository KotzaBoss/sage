section_start("Compiler")

# Assertions
if (NOT DEFINED CMAKE_C_COMPILER)
	section_fatal("C compiler not defined")
elseif(NOT DEFINED CMAKE_CXX_COMPILER)
	section_fatal("CXX compiler not defined")
endif()

# Expectations
set(EXPECTED_COMPILER_ID "GNU")
set(EXPECTED_COMPILER_VERSION "13.0.0")
set(EXPECTED_COMPILER_DEBUG "${EXPECTED_COMPILER_ID} ${EXPECTED_COMPILER_VERSION}")

# Checks
list(APPEND langs		C							CXX)
list(APPEND compilers	${CMAKE_C_COMPILER}			${CMAKE_CXX_COMPILER})
list(APPEND ids			${CMAKE_C_COMPILER_ID}		${CMAKE_CXX_COMPILER_ID})
list(APPEND versions	${CMAKE_C_COMPILER_VERSION}	${CMAKE_CXX_COMPILER_VERSION})
foreach (lang compiler id version IN ZIP_LISTS langs compilers ids versions)
	section_message("${compiler} ${id} ${version}")
	if (NOT ${id} STREQUAL ${EXPECTED_COMPILER_ID})
		section_fatal("Expected compiler: ${EXPECTED_COMPILER_ID} found: ${id}")
	elseif(${version} VERSION_LESS ${EXPECTED_COMPILER_VERSION})
		section_fatal("Expected compiler version: ${EXPECTED_COMPILER_VERSION} found: ${version}")
	endif()
endforeach()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS TRUE)

add_compile_options(-Wall -Wextra -Wpedantic -pthread -fdiagnostics-color=always)

if (${CMAKE_CXX_COMPILER_VERSION} VERSION_GREATER_EQUAL 13)
	add_link_options(-latomic)
endif()

if (${CMAKE_BUILD_TYPE} STREQUAL Debug)
	add_compile_options(-fsanitize=undefined,address -fconcepts-diagnostics-depth=2)
	add_link_options(-fsanitize=undefined,address)
endif()

section_pass()
