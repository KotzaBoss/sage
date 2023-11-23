# Why this is not in standard cmake we will never know...
# At least global and target have the same length :D
define_property(GLOBAL PROPERTY DOC BRIEF_DOCS "Global documentation")
define_property(TARGET PROPERTY DOC BRIEF_DOCS "Target documentation")

function (docs)
	set(options SET GLOBAL ESCAPE_NEWLINE)
	set(one_value_args GET TARGET)
	set(multi_value_args DOCS)
	cmake_parse_arguments(DOCS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	validate_args()

	if (DOCS_SET)
		string(JOIN "\n" docs ${DOCS_DOCS})

		if (DOCS_GLOBAL)
			set_property(GLOBAL PROPERTY DOC ${docs})
		elseif (DOCS_TARGET)
			set_target_properties(${DOCS_TARGET} PROPERTIES DOC ${docs})
		else()
			message(FATAL_ERROR "Bad implementation")
		endif()

	elseif (DOCS_GET)
		if (DOCS_GLOBAL)
			get_property(get GLOBAL PROPERTY DOC)
		elseif (DOCS_TARGET)
			get_target_property(get ${DOCS_TARGET} DOC)
		else()
			message(FATAL_ERROR "Bad implementation")
		endif()

		if (DOCS_ESCAPE_NEWLINE)
			string(REPLACE "\n" "\\n" get ${get})
		endif()

		set(${DOCS_GET} "${get}" PARENT_SCOPE)

	else()
		message(FATAL_ERROR "Bad implementation")
	endif()
endfunction()

macro (validate_args)
	# Validate scope
	if (DOCS_GLOBAL)
		if (DOCS_TARGET)
			message(FATAL_ERROR "Only one of GLOBAL/TARGET xyz must be passed")
		endif()
	elseif (DOCS_TARGET)
		if (DOCS_GLOBAL)
			message(FATAL_ERROR "Only one of GLOBAL/TARGET xyz must be passed")
		endif()
	else()
		message(FATAL_ERROR "One of GLOBAL/TARGET xyz must be passed")
	endif()

	# Validate SET/GET
	if (DOCS_SET)
		if (DOCS_GET)
			message(FATAL_ERROR "Only one of SET or GET must be passed")
		elseif (NOT DOCS_DOCS)
			message(FATAL_ERROR "DOCS must be passed")
		endif()
	elseif (DOCS_GET)
		if (DOCS_SET)
			message(FATAL_ERROR "Only one of SET or GET must be passed")
		elseif (DOCS_DOCS)
			message(FATAL_ERROR "GET does not accept DOCS")
		endif()
	else()
		message(FATAL_ERROR "One of SET or GET must be passed")
	endif()
endmacro()

