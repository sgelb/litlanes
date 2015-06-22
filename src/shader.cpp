#include <string>
#include "shader.h"

Shader::Shader() {
  std::vector<GLuint> shaderIds_ = {};
}

// Load vertex- and fragmentshader from path
void Shader::load(const GLchar *shaderPath, GLenum shaderType) {
    std::string code;

  // Retrieve shader source code
  try {
    // Open files
    std::ifstream shaderFile(shaderPath);
    std::stringstream shaderStream;

    // Read files into streams
    shaderStream << shaderFile.rdbuf();

    // Close file handlers
    shaderFile.close();

    // Convert stream to GLchar array
    code = shaderStream.str();

  } catch (std::exception &e) {
    std::cerr << "Error: could not read file: " << e.what() << std::endl;
  }

  // Compile shaders
  GLint success;
  GLchar infoLog[512];

  const GLchar *shaderCode = static_cast<const GLchar *>(code.c_str());

  GLuint shaderId = glCreateShader(shaderType);
  glShaderSource(shaderId, 1, &shaderCode, NULL);
  glCompileShader(shaderId);
  // Print compile errors
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
    std::cerr << "Error: could not compile shader: " << infoLog << std::endl;
  }
  shaderIds_.push_back(shaderId);
}

void Shader::createProgram() {
  // Shader program
  program_ = glCreateProgram();
  for (GLuint id : shaderIds_) {
    glAttachShader(program_, id);
  }
  glLinkProgram(program_);

  // Print linking errors
  GLint success;
  GLchar infoLog[512];
  glGetProgramiv(program_, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(program_, 512, NULL, infoLog);
    std::cerr << "Error: could not link shaders: " << infoLog << std::endl;
  }

  // Delete shaders after linking
  for (GLuint id : shaderIds_) {
    glDeleteShader(id);
  }
  shaderIds_.clear();
}

void Shader::use() {
  createProgram();
  glUseProgram(program_);
}

GLuint Shader::getProgram() {
  return program_;
}
