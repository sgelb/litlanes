# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (litlanes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(OpenGL REQUIRED)


# Compile external dependencies 
add_subdirectory (external)


include_directories(
	external/glfw-3.1/include/
	external/glm-0.9.6.3/
	external/glew-1.12.0/include/
	external/libnoise-1.0.0/include
	include
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW
	noise-static
)

add_definitions(
	-DGLEW_STATIC
)

set(SOURCES 
	src/camera.cpp
	src/main.cpp
	src/shader.cpp
	src/terrain.cpp
)

set(HEADER
	include/camera.h
	include/constants.h
	include/shader.h
	include/terrain.h
)

set(SHADER
	shader/default.frag
	shader/default.vert
)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADER} ${SHADER})
target_link_libraries(${PROJECT_NAME}	${ALL_LIBS})

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$" )

