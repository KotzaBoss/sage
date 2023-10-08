section_start("Dependencies")
section_message("Each dependency may take a moment to prepare, don't be alarmed.")

include(FetchContent)

FetchContent_Declare(fmt
		GIT_REPOSITORY git@github.com:fmtlib/fmt.git
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(spdlog
		GIT_REPOSITORY https://github.com/gabime/spdlog
		GIT_TAG v1.x
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(glad
		GIT_REPOSITORY https://github.com/Dav1dde/glad
		GIT_TAG origin/glad2
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(glfw
		GIT_REPOSITORY https://github.com/glfw/glfw.git
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(imgui
		GIT_REPOSITORY https://github.com/ocornut/imgui.git
		GIT_TAG origin/docking
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(glm
		GIT_REPOSITORY https://github.com/g-truc/glm.git
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(stb
		GIT_REPOSITORY https://github.com/nothings/stb
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(doctest
		GIT_REPOSITORY https://github.com/doctest/doctest.git
		BUILD_ALWAYS OFF
	)

#

section_start("fmt")
FetchContent_MakeAvailable(fmt)
include_directories(${fmt_SOURCE_DIR}/include)
section_pass(${fmt_SOURCE_DIR})

section_start("spdlog")
set(SPDLOG_FMT_EXTERNAL ON)
FetchContent_MakeAvailable(spdlog)
include_directories(${spdlog_SOURCE_DIR}/include)
section_pass(${spdlog_SOURCE_DIR})

section_start("glad")
FetchContent_MakeAvailable(glad)
add_subdirectory(${glad_SOURCE_DIR}/cmake EXCLUDE_FROM_ALL)
glad_add_library(glad
	STATIC
	LOCATION ${glad_BINARY_DIR}/sources
	API gl:core=4.5
	)
include_directories(${glad_BINARY_DIR}/sources/include)
section_pass(${glad_SOURCE_DIR} ${glad_BINARY_DIR}/sources/include)

section_start("glfw")
FetchContent_MakeAvailable(glfw)
include_directories(${glfw_SOURCE_DIR}/include)
section_pass(${glfw_SOURCE_DIR})

section_start("imgui")
FetchContent_MakeAvailable(imgui)
include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/backends)
section_pass(${imgui_SOURCE_DIR})

section_start("glm")
FetchContent_MakeAvailable(glm)
include_directories(${glm_SOURCE_DIR})
section_pass(${glm_SOURCE_DIR})

section_start("stb")
FetchContent_MakeAvailable(stb)
section_message("Writting ${stb_SOURCE_DIR}/stb_image.cpp library")
file(WRITE
		${stb_SOURCE_DIR}/stb_image.cpp
		"#define STB_IMAGE_IMPLEMENTATION\n"
		"#include \"stb_image.h\"\n"
	)
add_library(stb STATIC ${stb_SOURCE_DIR}/stb_image.cpp)
include_directories(${stb_SOURCE_DIR})
section_pass(${stb_SOURCE_DIR})

section_start("doctest")
FetchContent_MakeAvailable(doctest)
include_directories(${doctest_SOURCE_DIR}/doctest)
section_pass(${doctest_SOURCE_DIR})


section_pass()
