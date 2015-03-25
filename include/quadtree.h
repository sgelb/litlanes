#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "boundingbox.h"
#include "constants.h"

class Quadtree {
 public:
  explicit Quadtree(const int &level = 0, const int &startpoint = 0);
  bool isLeaf();
  std::vector<GLuint> getIndicesOfLevel(const int &lod);


 private:
  int level_;
  int startpoint_;
  bool isLeaf_;
  std::unique_ptr<BoundingBox> boundingBox_;
  std::vector<GLuint> indices_;

  // 4 children: nw, sw, se, ne
  std::vector<std::shared_ptr<Quadtree>> children_;
};
