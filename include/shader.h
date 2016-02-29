#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

// Handle vertex and fragment shaders
class Shader {
 public:
  Shader();

  void load(const GLchar *shaderPath, GLenum shaderType);
  /* const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath); */
  void use();
  GLuint getProgram();

 private:
  GLuint program_;
  std::vector<GLuint> shaderIds_;

  void createProgram();
};
