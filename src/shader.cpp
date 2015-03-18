#include <string>
#include "shader.h"

Shader::Shader(const GLchar *vertexPath, const GLchar *fragmentPath) {
  std::string vertexCode;
  std::string fragmentCode;

  // Retrieve shader source code

  try {
    // Open files
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);
    std::stringstream vShaderStream;
    std::stringstream fShaderStream;

    // Read files into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    // Close file handlers
    vShaderFile.close();
    fShaderFile.close();

    // Convert stream to GLchar array
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::exception &e) {
    std::cout << "ERROR::SHADER::FILE_NOT_READ\n" << e.what() << std::endl;
  }
  const GLchar *vShaderCode = vertexCode.c_str();
  const GLchar *fShaderCode = fragmentCode.c_str();

  // Compile shaders
  GLuint vertex;
  GLuint fragment;
  GLint success;
  GLchar infoLog[512];

  // Vertex Shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vShaderCode, NULL);
  glCompileShader(vertex);
  // Print compile errors
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog
              << std::endl;
  }

  // Vertex Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fShaderCode, NULL);
  glCompileShader(fragment);
  // Print compile errors
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragment, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog
              << std::endl;
  }

  // Shader program
  program_ = glCreateProgram();
  glAttachShader(program_, vertex);
  glAttachShader(program_, fragment);
  glLinkProgram(program_);
  // Print linking errors
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog
              << std::endl;
  }

  // Delete shaders after linking
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() {
  glUseProgram(program_);
}

GLuint Shader::getProgram() {
  return program_;
}
