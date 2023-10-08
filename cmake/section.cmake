macro(section_message msg)
	message(STATUS "${msg}")
endmacro()

macro(section_indent)
	if (NOT CMAKE_MESSAGE_INDENT)
		list(APPEND CMAKE_MESSAGE_INDENT "    ")
	endif()
endmacro()

macro(section_unindent)
	if (CMAKE_MESSAGE_INDENT)
		list(POP_BACK CMAKE_MESSAGE_INDENT)
	endif()
endmacro()

macro(section_start msg)
	section_unindent()
	message(CHECK_START ${msg})
	section_indent()
endmacro()

macro(section_pass)
	section_unindent()
	if (${ARGC} GREATER 0)
		string(JOIN " " msg ${ARGV})
		message(CHECK_PASS "${msg}")
	else()
		message(CHECK_PASS "Ok")
	endif()
endmacro()

macro(section_fail msg)
	section_unindent()
	message(CHECK_FAIL ${msg})
endmacro()

macro(section_fatal msg)
	message(FATAL_ERROR ${msg})
endmacro()
