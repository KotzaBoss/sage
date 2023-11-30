# Make sure external_docs and make_readme are called at the end of your cmake configuration
# so that the customisations, like sage_options, have been declared.

# Why this is not in standard cmake we will never know...
# At least global and target have the same length :D
define_property(GLOBAL PROPERTY DOC BRIEF_DOCS "Global documentation property")
define_property(TARGET PROPERTY DOC BRIEF_DOCS "Target documentation property")

function (docs)
	set(options SET GLOBAL ESCAPE_NEWLINE)
	set(one_value_args GET TARGET PREPEND)
	set(multi_value_args DOCS)
	cmake_parse_arguments(DOCS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	_validate_args()

	if (DOCS_SET)
		if (DOCS_GLOBAL)
			set_property(GLOBAL PROPERTY DOC "${DOCS_DOCS}")
		elseif (DOCS_TARGET)
			set_target_properties(${DOCS_TARGET} PROPERTIES DOC "${DOCS_DOCS}")
		else()
			message(FATAL_ERROR "Bad implementation")
		endif()

	elseif (DOCS_GET)
		if (DOCS_GLOBAL)
			get_property(doc GLOBAL PROPERTY DOC)
		elseif (DOCS_TARGET)
			get_target_property(doc ${DOCS_TARGET} DOC)
		else()
			message(FATAL_ERROR "Bad implementation")
		endif()

		if (DOCS_PREPEND)
			list(TRANSFORM doc PREPEND ${DOCS_PREPEND})
		endif()

		list(JOIN doc "\n" doc)

		if (DOCS_ESCAPE_NEWLINE)
			string(REPLACE "\n" "\\n" doc ${doc})
		endif()

		set(${DOCS_GET} "${doc}" PARENT_SCOPE)

	else()
		message(FATAL_ERROR "Bad implementation")
	endif()
endfunction()

set(_glob "*.cpp" "*.h" "*.hpp" "*.cmake")

sage_options(ADD OBSIDIAN DOC "Tell SAGE to use Gkiwnis' [.obsidian](https://obsidian.md/)" INIT ON)
function (external_docs)
	#set(options )
	set(one_value_args OUTPUT_DIR)
	set(multi_value_args PROJECT_DIRS)
	cmake_parse_arguments(EXT_DOCS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	if (NOT EXT_DOCS_PROJECT_DIRS)
		message(FATAL_ERROR "Expected PROJECT_DIRS")
	endif()

	if (NOT EXT_DOCS_OUTPUT_DIR)
		set(EXT_DOCS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	if (SAGE_OPT_OBSIDIAN)
		file(COPY ${PROJECT_SOURCE_DIR}/.obsidian DESTINATION ${EXT_DOCS_OUTPUT_DIR})
	endif()

	# Prepare build/docs directories
	foreach(dir ${EXT_DOCS_PROJECT_DIRS})
		file(MAKE_DIRECTORY ${EXT_DOCS_OUTPUT_DIR}/${dir})
	endforeach()

	list(TRANSFORM EXT_DOCS_PROJECT_DIRS PREPEND "${PROJECT_SOURCE_DIR}/")

	foreach (dir ${EXT_DOCS_PROJECT_DIRS})
		list(TRANSFORM _glob PREPEND "${dir}/" OUTPUT_VARIABLE glob)
		file(GLOB_RECURSE dir_files ${glob})
		foreach (file ${dir_files})
			block(PROPAGATE dir file)
				cmake_path(GET file EXTENSION ext)

				# For code files, collect #include links
				if (NOT ext STREQUAL ".cmake")
					file(STRINGS ${file} code)
					# Find links to other files
					foreach (line in ${code})
						string(REGEX MATCH "#include \"(.*\.h(pp)?)\"" match ${line})
						if (match)
							list(APPEND links "  - \"[[${CMAKE_MATCH_1}.md]]\"")	# .md to differentiate the code from the note
						endif()
					endforeach()
				endif()

				# Collect tags
				# FIXME: for platform/linux/sage.hpp it puts two `#linux` tags
				cmake_path(GET dir FILENAME dir)
				list(APPEND tags "  - \"#${dir}\"")
				if (${file} MATCHES ".*/linux/.*")
					list(APPEND tags "  - \"#linux\"")
				endif()

				cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE dest)

				# Make symbolic link to source file
				file(CREATE_LINK ${file} ${EXT_DOCS_OUTPUT_DIR}/${dest} SYMBOLIC)

				# Write content
				string(JOIN "\n" content
						"---"
						"tags:"
						${tags}
						"links:"
						${links}
						"---"
						"![[${dest}]]"
					)
				file(WRITE ${EXT_DOCS_OUTPUT_DIR}/${dest}.md ${content})
			endblock()
		endforeach()
	endforeach()
endfunction()

function (make_readme)
	#set(options)
	set(one_value_args OUTPUT_DIR)
	set(multi_value_args PROJECT_DIRS)
	cmake_parse_arguments(EXT_DOCS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	if (NOT EXT_DOCS_PROJECT_DIRS)
		message(FATAL_ERROR "Expected PROJECT_DIRS")
	endif()

	if (NOT EXT_DOCS_OUTPUT_DIR)
		set(EXT_DOCS_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	set(toc "%% [toc] At some point it will be supported by Obsidian %%")
	list(APPEND options "| Option | Doc |" "|:-|:-|")
	sage_options(GET ALL OUT opts DOC docs)
	foreach(opt_doc IN ZIP_LISTS opts docs)
		list(APPEND options "|`${opt_doc_0}`| ${opt_doc_1}|")
	endforeach()
	list(JOIN options "\n" options)

	# -P ${glob} doesnt seem to work right so just use .gitignore
	#string(JOIN "|" glob ${_glob})
	#string(CONCAT glob \" ${glob} \")
	set(size_regex "[0-9a-zA-Z.]+")
	execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMAND tree -s -h --du --gitignore --dirsfirst -F #${EXT_DOCS_PROJECT_DIRS}
			OUTPUT_VARIABLE tree
		)
	string(REGEX REPLACE "${size_regex} used in [0-9]+ directories, [0-9]+ files" "" tree ${tree})
	set(tree_summary ${CMAKE_MATCH_0})
	string(REGEX REPLACE "\\[[ ]*${size_regex}\\]  " "" tree ${tree})
	string(STRIP ${tree} tree)

	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/README.md.in ${EXT_DOCS_OUTPUT_DIR}/README.md)

	file(READ ${EXT_DOCS_OUTPUT_DIR}/README.md content)
	obsidian_to_normal_markdown(content ${content})
	file(WRITE ${PROJECT_SOURCE_DIR}/README.md ${content})
endfunction()

set(_todos TODO FIXME OPTIMIZE CONFUSION)
set(_note_regex "[a-zA-Z0-9_/.^]+")
set(_section_regex "#[a-zA-Z0-9_ -]+")

function(make_todo)
	set(one_value_args OUTPUT_DIR)
	set(multi_value_args PROJECT_DIRS)
	cmake_parse_arguments(TODO "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	if (NOT TODO_PROJECT_DIRS)
		message(FATAL_ERROR "Expected PROJECT_DIRS")
	endif()

	list(TRANSFORM TODO_PROJECT_DIRS PREPEND "${PROJECT_SOURCE_DIR}/")

	if (NOT TODO_OUTPUT_DIR)
		set(TODO_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR})
	endif()

	block()
		string(JOIN "|" comment_token "//" "#")
		string(JOIN "|" todos ${_todos})
		set(todo "(${comment_token})[ ]*(${todos}):? ")

		foreach(dir ${TODO_PROJECT_DIRS})
			list(TRANSFORM _glob PREPEND "${dir}/" OUTPUT_VARIABLE glob)
			file(GLOB_RECURSE files ${glob})
			foreach(file ${files})
				file(STRINGS ${file} lines)
				cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")

				list(LENGTH lines len)
				math(EXPR len "${len} - 1")		# Thanks cmake...
				foreach(i RANGE 0 ${len})
					list(GET lines ${i} line)

					if (NOT line)
						continue()
					endif()

					string(REGEX MATCH ${todo} match ${line})
					if (NOT match)
						continue()
					endif()

					#list(FILTER lines INCLUDE REGEX ${todo})

					string(STRIP ${line} line)

					#list(TRANSFORM lines STRIP)

					# Escape template syntax because markdown thinks its html, yes we do need 4 backslashes...
					string(REGEX REPLACE "<([a-zA-Z0-9_:]+)>" "\\\\<\\1\\\\>" line ${line})
					#list(TRANSFORM lines REPLACE "<([a-zA-Z0-9_:]+)>" "\\\\<\\1\\\\>")

					math(EXPR line_number "${i} + 1")
					string(REGEX REPLACE "${todo}" "\\2 | [[${file}]] | ${line_number} | " line ${line})
					#list(TRANSFORM lines REPLACE "${todo}" "\\2 | [[${file}]] | ")	# \\2 (todo type) will be consumed as we iterate the todo types

					list(APPEND content ${line})
					#list(APPEND content ${lines})
				endforeach()
			endforeach()
		endforeach()

		list(REMOVE_DUPLICATES content)

		# Consume todo type and dump them in separate lists
		while (content)
			list(POP_BACK content line)
			string(REGEX REPLACE "(${todos}) \\| \\[\\[(${_note_regex})\\]\\] \\| ([0-9]+) \\| (.*)" "[[\\2]] | \\3 | \\4" line ${line})
			assert(COND ${CMAKE_MATCH_COUNT} GREATER 0 MSG "Regex and string generation do not seem to be agreeing")
			list(APPEND ${CMAKE_MATCH_1} ${line})

			string(JOIN "\n" todo_note
					"---"
					"tags:"
					"  - bugs"
					"  - ${CMAKE_MATCH_1}"
					"links:"
					"---"
					"[[${CMAKE_MATCH_2}.md]]"
				)

			# CMAKE_MATCH_2 contains the project directory: bin/main.cpp, test/...
			set(filename ${CMAKE_MATCH_2})
			cmake_path(GET filename FILENAME fname)
			string(REPLACE ${fname} "bug__${fname}" filename ${filename})
			file(WRITE ${TODO_OUTPUT_DIR}/${filename}__${CMAKE_MATCH_3}.md ${todo_note})
		endwhile()

		set(total_todos 0)
		foreach (td ${_todos})
			list(APPEND content "## ${td}")
			list(LENGTH ${td} len)
			math(EXPR total_todos "${total_todos} + ${len}")
			if (${len} GREATER 0)
				list(TRANSFORM ${td} PREPEND "| ")
				list(TRANSFORM ${td} APPEND " |")
				list(APPEND content
						"${len} items available.\n"
						"| Source | Line number | Description |"
						"|:-|:-|:-|"
						${${td}}
					)
			else()
				list(APPEND content "No items available.")
			endif()
		endforeach()

		list(PREPEND content
				"# Overview"
				"${total_todos} items to be done."
			)

		string(JOIN "\n" content ${content})
		file(WRITE ${TODO_OUTPUT_DIR}/TODO.md ${content})

		obsidian_to_normal_markdown(content ${content})
		file(WRITE ${PROJECT_SOURCE_DIR}/TODO.md ${content})
	endblock()
endfunction()

# Utilities

function(obsidian_to_normal_markdown out content)
	# Links
	set(regex "\\[\\[(${_note_regex})(${_section_regex})?\\]\\]")

	string(REGEX MATCHALL ${regex} links ${content})							# [ "[[README#Project Overview]]", "[[bin/main.cpp]]", ... ]
	foreach(link ${links})
		block(PROPAGATE content regex link)
			string(REGEX MATCH ${regex} match ${link})
			assert(COND match MSG "Should always match since the links already matched the regex")

			set(visible ${CMAKE_MATCH_1})										# [README]()	[bin/main.cpp]()
			cmake_path(GET link EXTENSION ext)
			if (NOT ext)
				set(note ${visible}.md)
			else()
				set(note ${visible})
			endif()																# [README](README.md)	[bin/main.cpp](bin/main.cpp)

			# Section
			if (CMAKE_MATCH_2)
				set(section ${CMAKE_MATCH_2})
				string(APPEND visible ${section})								# [README#Project Overview](README.md#Project Overview) [bin/main.cpp](bin/main.cpp)
				string(TOLOWER ${section} processed_section)					# [README#Project Overview](README.md#project overview)	[bin/main.cpp](bin/main.cpp)
				string(REPLACE " " "-" processed_section ${processed_section})	# [README#Project Overview](README.md#project-overview)	[bin/main.cpp](bin/main.cpp)
			endif()

			set(to_replace "[${visible}](${note}${processed_section})")
			string(REPLACE ${link} ${to_replace} content ${content})
		endblock()
	endforeach()

	# Comments
	string(REGEX REPLACE "%%(.*)%%" "<!-- \\1 -->" content ${content})

	# Github does not support alert/callout titles
	string(REGEX REPLACE "> \\[!(.*)\\][a-zA-Z0-9 .-_]+\n" "> [!\\1]\n" content ${content})

	set(${out} ${content} PARENT_SCOPE)
endfunction()

macro (_validate_args)
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

