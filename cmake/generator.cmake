section_start("Generator")

if (NOT ${CMAKE_GENERATOR} MATCHES "Ninja")
	message(WARNING "Consider configuring cmake with `-G Ninja` for better performance")
endif()

section_pass("${CMAKE_GENERATOR}")
