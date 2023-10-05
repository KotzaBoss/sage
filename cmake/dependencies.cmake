section_start("Dependencies")

section_next("fmt")
add_subdirectory(fmt EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/fmt/include)

section_next("spdlog")
set(SPDLOG_FMT_EXTERNAL ON)
add_subdirectory(spdlog EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/spdlog/include)

section_next("doctest")
add_subdirectory(doctest EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/doctest/doctest)

section_next("glad")
add_subdirectory(glad/cmake EXCLUDE_FROM_ALL)
glad_add_library(glad
	STATIC
	LOCATION ${CMAKE_CURRENT_BINARY_DIR}/glad/sources
	API gl:core=4.5
	)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/glad/sources/include)

section_next("glfw")
add_subdirectory(glfw EXCLUDE_FROM_ALL)
include_directories(${PROJECT_SOURCE_DIR}/glfw/include)

section_next("imgui")
include_directories(${PROJECT_SOURCE_DIR}/imgui)
include_directories(${PROJECT_SOURCE_DIR}/imgui/backends)

section_next("glm")
add_subdirectory(glm)
include_directories(${PROJECT_SOURCE_DIR}/glm)

section_next("stb")
add_subdirectory(stb)
include_directories(${PROJECT_SOURCE_DIR}/stb)

section_pass("Ok")
