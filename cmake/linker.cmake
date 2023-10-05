section_start("Linker")

set(faster_linkers "lld" "mold")
cmake_path(GET CMAKE_LINKER FILENAME linker)
if (NOT ${linker} IN_LIST faster_linkers)
	message(WARNING
		"Consider configuring cmake with `-DCMAKE_LINKER=\"` with one of ${faster_linkers} to speed up linking")
endif()

# CMake seems to just invoke the compiler even for linking so this way we force the linker
add_compile_options(-fuse-ld=$<PATH:GET_FILENAME,${CMAKE_LINKER}>)

section_pass("Ok")
