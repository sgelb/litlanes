#pragma once

#include <vector>
#include <iostream>
#include <memory>

#include <noise/noise.h>
#include <GLFW/glfw3.h>

#include "constants.h"

class Terrain {
 public:
  explicit Terrain(GLuint meshSize = Constants::MeshWidth);
  ~Terrain();
  std::vector<GLfloat> getVertices();
  std::vector<GLuint> getIndices();
  void setAlgorithm(int algorithm);
  void create();

 private:
  GLuint meshSize_;
  std::vector<GLfloat> vertices_;
  std::vector<GLuint> indices_;
  noise::module::Module *noise_;
  void createVertices();
  void createIndices();
  GLfloat mapToInterval(const GLfloat &input);
  std::vector<GLfloat> colorFromHeight(GLfloat height);
};
