# get_directory_properties(... SUBDIRECTORIES) does not recurse,
# how to do that elegantly?
set(project_dirs
		asset
		asset/texture
		asset/shader
		cmake
		src
		src/platform
		src/platform/linux
		bin
		test
		perf
		docs
	)
list(SORT project_dirs)

external_docs(PROJECT_DIRS ${project_dirs})
make_readme(PROJECT_DIRS ${project_dirs})
make_todo(PROJECT_DIRS ${project_dirs} TAGS TODO FIXME CONFUSION RND OPTIMIZE)

