# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (litlanes)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# Options. Turn on/off with 'cmake -Dmyvarname=ON/OFF' 

# Build tests
option(BUILD_TESTS "Build all tests" ON)



# Program libraries and executables

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)


include_directories(
  external/glfw-3.1/include/
  external/glm-0.9.6.3/
  external/glew-1.12.0/include/
  external/libnoise-1.0.0/include
  include
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
  src/quadtree.cpp
  src/shader.cpp
  src/terrain.cpp
  )

set(HEADER
  include/camera.h
  include/constants.h
  include/quadtree.h
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

SOURCE_GROUP(shaders REGULAR_EXPRESSION ".*/.*shader$")

# Test library and executables

if (BUILD_TESTS)

  set(TESTS
    test/testQuadtree.cpp
    test/testTerrain.cpp
    )

  set(TEST_SOURCES
    src/quadtree.cpp
    src/terrain.cpp
    )

  set(TEST_HEADER
    include/constants.h
    include/quadtree.h
    include/terrain.h
    )

  add_subdirectory(external/gtest-1.7.0)
  enable_testing()
  include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
  add_executable(runTests ${TESTS} ${TEST_SOURCES} ${TEST_HEADER})
  target_link_libraries(runTests gtest gtest_main ${ALL_LIBS})
endif (BUILD_TESTS)
