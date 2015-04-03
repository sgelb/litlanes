#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/**
 * @brief Handle vertex and fragment shaders
 */
class Shader {
 public:
   /**
    * @brief Load and compile shader files
    *
    * @param vertexSourcePath Path to vertex shader file
    * @param fragmentSourcePath Path to fragment shader file
    */
  Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
  /**
   * @brief Use this shader program in OpenGL
   */
  void use();
  /**
   * @brief Return shader program
   *
   * @return Shader program compiled from vertex and fragment shader sources
   */
  GLuint getProgram();

 private:
  GLuint program_;
};
