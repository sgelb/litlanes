#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>

#include "boundingbox.h"
#include "constants.h"

/**
 * @brief Data structure for partitioning tile in different levels of detail
 */
class Quadtree {
 public:
   /**
    * @brief Constructor 
    *
    * @param level Current level of detail
    * @param startpoint Index of top-left point of quadtree in vertex indices 
    * array
    */
  explicit Quadtree(const int &level = 0, const int &startpoint = 0);

  /**
   * @brief A Quadtree is a leaf if it has no children
   *
   * @return \c True if Quadtree is a leaf, \c False otherwise
   */
  bool isLeaf();

  /**
   * @brief Return vertex indices for provided level of detail
   *
   * @param lod level of detail
   *
   * @return vertex indices of provided level of detail
   */
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
