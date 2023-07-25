message(STATUS "Copying shaders to ${CMAKE_CURRENT_BINARY_DIR}")
file(GLOB shaders *.glsl)
file(COPY ${shaders} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
cmake_print_variables(shaders)
