function (assert)
	cmake_parse_arguments(ASSERT "" "MSG" "COND" ${ARGN})
	if (NOT ASSERT_COND)
		message(FATAL_ERROR "CONDition to assert required")
	endif()

	string(JOIN " " ASSERT_COND ${ASSERT_COND})
	cmake_language(EVAL CODE "
			if (NOT (${ASSERT_COND}))
				message(FATAL_ERROR \"Assertion failed: `${ASSERT_COND}`\n${ASSERT_MSG}\n\")
			endif()
			"
		)
endfunction()

