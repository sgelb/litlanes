#pragma once

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
 public:
  Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
  void use();
  GLuint getProgram();

 private:
  GLuint program_;
};
