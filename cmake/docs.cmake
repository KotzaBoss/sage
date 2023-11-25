# Make sure external_docs and make readme are called at the end of your cmake configuration
# so that the customisations, like sage_options, have been declared.

# Why this is not in standard cmake we will never know...
# At least global and target have the same length :D
define_property(GLOBAL PROPERTY DOC BRIEF_DOCS "Global documentation property")
define_property(TARGET PROPERTY DOC BRIEF_DOCS "Target documentation property")

function (docs)
	set(options SET GLOBAL ESCAPE_NEWLINE)
	set(one_value_args GET TARGET)
	set(multi_value_args DOCS)
	cmake_parse_arguments(DOCS "${options}" "${one_value_args}" "${multi_value_args}" ${ARGN})

	_validate_args()

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

		set(${DOCS_GET} "${get}")
		return(PROPAGATE ${DOCS_GET})

	else()
		message(FATAL_ERROR "Bad implementation")
	endif()
endfunction()

set(_glob "*.cpp" "*.h" "*.hpp" "*.cmake")

sage_options(ADD OBSIDIAN DOC "Tell SAGE to prepare material for [Obsidian](https://obsidian.md/) (see `external_docs` in [[cmake/docs.cmake]])" INIT ON)
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
		set(snippets ${EXT_DOCS_OUTPUT_DIR}/.obsidian/snippets)
		file(MAKE_DIRECTORY ${snippets})

		# Justification
		set(justify ${snippets}/justify.css)
		string(JOIN "\n" justify_content
				"/* reading mode */"
				".markdown-preview-view p {"
				"	text-align: justify;"
				"	text-justify: inter-word;"
				"}"
				""
				"/* source view and live preview */"
				".markdown-source-view.mod-cm6 .cm-line {"
				"	text-align: justify;"
				"	text-justify: inter-word;"
				"}"
				""
			)
		file(WRITE ${justify} ${justify_content})
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

	list(APPEND options "| Option | Doc |" "|:-|:-|")
	sage_options(GET ALL OUT opts DOC docs)
	foreach(opt_doc IN ZIP_LISTS opts docs)
		list(APPEND options "|`${opt_doc_0}`| ${opt_doc_1}|")
	endforeach()

	# -P ${glob} doesnt seem to work right so just use .gitignore
	#string(JOIN "|" glob ${_glob})
	#string(CONCAT glob \" ${glob} \")
	execute_process(
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMAND tree --gitignore --sort=name #${EXT_DOCS_PROJECT_DIRS}
			OUTPUT_VARIABLE tree
		)

	string(JOIN "\n" content
			"[toc] %% At some point it will be supported by Obsidian %%"
			""

			"# SAGE: *Super Advanced Game Engine*"
			""

			"> [!TIP] When in doubt..."
			"> `cmake --build build -- usage`"
			""

			"## Getting Started"
			"```"
			"cmake -B build -D SAGE_OPT_CCACHE_ENABLED=ON"
			"```"
			"To customize the build see [[README#Build]]."
			""
			"Once you have a successful build you can begin reading [[bin/main.cpp]] and \
looking at the tests to get a feel of the project."
			""

			"## Development"
			"A big part of SAGE is automated. \
When you are done tweaking/extending/refactoring, make sure you \
run `cmake` at least once to generate docs and specifically the [[README]]."
			""

			"### Build"
			"```"
			"cmake -B build	\\"
			"      -G Ninja	\\"
			"      -D SAGE_OPT_CCACHE_ENABLED=ON	\\"
			"      -D CMAKE_LINKER=mold"
			"```"
			${options}
			""

			"## Project Overview"
			"```"
			${tree}
			"```"
			""
		)
	file(WRITE ${EXT_DOCS_OUTPUT_DIR}/README.md ${content})

	# Replace Obsidian specific syntax with normal markdown for the top level README

	# Links
	set(note_regex "[a-zA-Z0-9_/.^]+")
	set(section_regex "#[a-zA-Z0-9_ -]+")
	set(regex "\\[\\[(${note_regex})(${section_regex})?\\]\\]")

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

	file(WRITE ${PROJECT_SOURCE_DIR}/README.md ${content})
endfunction()

# Utilities

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

