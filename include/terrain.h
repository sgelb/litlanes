#pragma once

#include <vector>
#include <iostream>
#include <memory>

#include <noise/noise.h>
#include <GLFW/glfw3.h>

#include "constants.h"


class Terrain {
 public:
  explicit Terrain(GLuint tileWidth = Constants::TileWidth);
  std::vector<GLfloat> getVertices();
  std::vector<GLuint> getIndices();
  void setAlgorithm(const int &algorithm);
  void create();

 private:
  GLuint tileWidth_;
  std::unique_ptr<noise::module::Module> noise_;
  std::vector<GLfloat> vertices_;
  std::vector<GLuint> indices_;
  void createVertices();
  void createIndices();
  GLfloat mapToInterval(const GLfloat &input);
  std::vector<GLfloat> colorFromHeight(const GLfloat &height);
};
