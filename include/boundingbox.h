#pragma once

#include <glm/glm.hpp>

/**
 * @brief Simplified Axis-Aligned Square Bounding Box (AABB) using a cube
 */
class BoundingBox {
 public:
   /**
    * @brief Constructor
    *
    * @param center center of AABB
    * @param extents length from center to faces which is half the length of an 
    * cube edge
    */
  BoundingBox(glm::vec3 center, float extents);

  /**
   * @brief Test if sphere intersects with bounding box
   *
   * @param spherePosition position of sphere
   * @param sphereRadius radius of sphere
   *
   * @return \c True if sphere intersects with bounding box, \c False otherwise
   */
  bool intersectsWithSphere(const glm::vec3 &spherePosition,
      const float &sphereRadius);

 private:
  glm::vec3 center_;
  float extents_;
};
