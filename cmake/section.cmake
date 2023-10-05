set(section_tab_character "\t")

macro(section_tab)
	list(APPEND CMAKE_MESSAGE_INDENT "${section_tab_character}")
endmacro()

macro(section_backspace)
	list(POP_BACK CMAKE_MESSAGE_INDENT)
endmacro()

macro(section_message msg)
	message(STATUS ${msg})
endmacro()

macro(section_start msg)
	message(CHECK_START ${msg})
	section_tab()
endmacro()

macro(section_next msg)
	section_backspace()
	message(STATUS ${msg})
	section_tab()
endmacro()

macro(section_pass msg)
	section_backspace()
	message(CHECK_PASS ${msg})
endmacro()

macro(section_fail msg)
	section_backspace()
	message(CHECK_FAIL ${msg})
endmacro()

macro(section_fatal msg)
	message(FATAL_ERROR ${msg})
endmacro()
