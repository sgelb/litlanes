#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <noise/noise.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>  // fastNormalize

#include "constants.h"


struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
};


class Terrain {
 public:
  explicit Terrain(GLuint tileWidth = Constants::TileWidth);
  std::vector<Vertex> getVertices();
  std::vector<GLuint> getIndices();
  void setAlgorithm(const int &algorithm);
  void create();

 private:
  using modulePtr = std::unique_ptr<noise::module::Module>;
  GLuint tileWidth_;
  modulePtr noise_;
  std::vector<Vertex> vertices_;
  std::vector<GLuint> indices_;
  GLuint verticesCount_;

  void createVertices();
  void createHeightMap();
  void createNormals();
  void createIndices();
  GLfloat mapToInterval(const GLfloat &input);
  std::vector<GLfloat> colorFromHeight(const GLfloat &height);
};
