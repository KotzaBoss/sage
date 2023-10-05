section_start("Subdirectories")

include_directories(${PROJECT_SOURCE_DIR})

section_next("src")
include_directories(${PROJECT_SOURCE_DIR}/src)
add_subdirectory(src)

section_next("asset")
add_subdirectory(asset)

section_next("test")
add_subdirectory(test)

section_next("sandbox")
option(SAGE_BUILD_SANDBOX "Build the sandbox executable found in the sandbox directory" ON)
if (${SAGE_BUILD_SANDBOX})
	add_subdirectory(sandbox)
endif()

section_pass("Ok")
