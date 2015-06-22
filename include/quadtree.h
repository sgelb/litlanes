#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "defaults.h"

// Data structure for partitioning tile in different levels of detail
// see https://en.wikipedia.org/wiki/Quadtree
class Quadtree {
 public:
  explicit Quadtree(const int &level = 0, const int &startpoint = 0);
  std::vector<GLuint> getIndicesOfLevel(const int &lod);

 private:
  int level_;
  int startpoint_;
  bool isLeaf_;
  std::vector<GLuint> indices_;
  std::vector<std::shared_ptr<Quadtree>> children_;

  bool isLeaf();
};
