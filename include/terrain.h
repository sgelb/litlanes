#pragma once

#include <vector>
#include <iostream>

#include <noise/noise.h>
#include <GLFW/glfw3.h>

#include "constants.h"

class Terrain {
 public:
  explicit Terrain(GLuint meshSize = Constants::MeshWidth);
  std::vector<GLfloat> getVertices();
  std::vector<GLuint> getIndices();

 private:
  GLuint meshSize_;
  std::vector<GLfloat> vertices_;
  std::vector<GLuint> indices_;
  void createVertices();
  void createIndices();
  GLfloat mapToInterval(const GLfloat &input);
};
