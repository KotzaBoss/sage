section_start("Dependencies")
section_message("")
section_message("Each dependency may take a moment to prepare, don't be alarmed.")
section_message("")

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

FetchContent_Declare(imgui
		GIT_REPOSITORY https://github.com/ocornut/imgui.git
		GIT_TAG origin/docking
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(raylib
		GIT_REPOSITORY https://github.com/raysan5/raylib.git
		BUILD_ALWAYS OFF
	)

FetchContent_Declare(rlImGui_project
		GIT_REPOSITORY https://github.com/raylib-extras/rlImGui.git
		GIT_TAG main
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
# Not the official way but only one that seems to work consistently and
# i have no interest in bothering more...
add_compile_definitions(
		SPDLOG_ENABLE_PCH
		SPDLOG_BUILD_PIC
		SPDLOG_FMT_EXTERNAL
		SPDLOG_FMT_NO_EXCEPTIONS
	)
FetchContent_MakeAvailable(spdlog)
include_directories(${spdlog_SOURCE_DIR}/include)
section_pass(${spdlog_SOURCE_DIR})

section_start("imgui")
FetchContent_MakeAvailable(imgui)
include_directories(${imgui_SOURCE_DIR})
include_directories(${imgui_SOURCE_DIR}/backends)
section_pass(${imgui_SOURCE_DIR})

section_start("raylib")
FetchContent_MakeAvailable(raylib)
include_directories(${raylib_SOURCE_DIR}/src)
section_pass(${raylib_SOURCE_DIR})

section_start("rlImGui")
FetchContent_MakeAvailable(rlImGui_project)
add_library(rlimgui STATIC
		${rlimgui_project_SOURCE_DIR}/rlImGui.cpp
		${imgui_SOURCE_DIR}/imgui.cpp
		${imgui_SOURCE_DIR}/imgui_draw.cpp
		${imgui_SOURCE_DIR}/imgui_tables.cpp
		${imgui_SOURCE_DIR}/imgui_widgets.cpp
		${imgui_SOURCE_DIR}/imgui_demo.cpp
	)
include_directories(${rlimgui_project_SOURCE_DIR})
section_pass(${rlimgui_project_SOURCE_DIR})

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
