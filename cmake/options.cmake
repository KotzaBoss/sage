function (sage_options)
	set(prefix "SAGE_OPT_")
	#set(options)
	set(one_value_args ADD GET OUT DOC INIT)
	#set(multi_value_args)
	cmake_parse_arguments(OPT "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	if (OPT_GET)
		if (OPT_ADD)
			message(FATAL_ERROR "Only one of GET/ADD must be passed")
		elseif (OPT_INIT)
			message(FATAL_ERROR "INIT not expected when GETting")
		elseif (NOT OPT_OUT)
			message(FATAL_ERROR "Expected OUT variable")
		endif()

		get_cmake_property(vars VARIABLES)
		list(FILTER vars INCLUDE REGEX "${prefix}.*")
		list(SORT vars)
		foreach (v ${vars})
			get_property(doc CACHE ${v} PROPERTY HELPSTRING)
			list(APPEND docs ${doc})
		endforeach()

		if (${OPT_GET} STREQUAL ALL)
			set(${OPT_OUT} ${vars})
			if (OPT_DOC)
				set(${OPT_DOC} ${docs})
			endif()

		else()
			_validate_option(${OPT_GET})

			set(opt "${prefix}${OPT_GET}")

			list(FIND vars ${opt} index)
			if (NOT ${index} EQUAL -1)
				list(GET vars ${index} value)
				list(GET docs ${index} doc)
				set(${OPT_OUT} ${value})
				set(${OPT_DOC} ${doc})
			else()
				message(FATAL_ERROR "${opt} is not an option")
			endif()
		endif()

		return (PROPAGATE ${OPT_OUT} ${OPT_DOC})

	elseif(OPT_ADD)
		_validate_option(${OPT_ADD})

		if (OPT_GET)
			message(FATAL_ERROR "Only one of GET/ADD must be passed")
		elseif (OPT_OUT)
			message(FATAL_ERROR "OUT variable not expected when ADDing")
		elseif (NOT DEFINED OPT_INIT)
			message(FATAL_ERROR "INITial value not passed for option ${OPT_ADD}")
		endif()

		if (NOT OPT_DOC)
			message(WARNING "Option ${OPT_ADD} does not have documentation, pass \"DOC TRIVIAL\" to suppress")
		endif()

		if (${OPT_DOC} STREQUAL TRIVIAL)
			unset(OPT_DOC)
		endif()

		option("${prefix}${OPT_ADD}" "${OPT_DOC}" ${OPT_INIT})

	endif()
endfunction()

function (_validate_option opt)
	if (${opt} MATCHES "SAGE_.*")
		message(FATAL_ERROR "Options are prefixed with \"${prefix}\" by default")
	endif()
endfunction()

