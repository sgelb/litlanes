#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <cmath>

#include "constants.h"

class Quadtree {
 public:
  Quadtree(const int &level, const int &startpoint);
  std::vector<GLuint> getIndices();
  bool isLeaf();

 private:
  int level_;
  int startpoint_;
  /* std::shared_ptr<Quadtree> parent; */
  bool isLeaf_;
  std::vector<GLuint> indices_;

  // 4 children: nw, sw, se, ne
  std::vector<std::shared_ptr<Quadtree>> children_;
  /* // 4 neighbors: n, w, s, e */
  /* std::vector<std::shared_ptr<Quadtree>> neighbors_; */

  /* glm::vec3 center_; */
  /* glm::vec3 nw_; */
  /* glm::vec3 sw_; */
  /* glm::vec3 se_; */
  /* glm::vec3 ne_; */
};
