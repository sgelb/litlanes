#ifndef SHADER_H_
#define SHADER_H_

#include <GL/glew.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {

public:
  GLuint Program;
  Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
  void Use();
};

#endif // SHADER_H_
