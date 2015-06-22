#pragma once

#include <glm/glm.hpp>

// Simplified Axis-Aligned Square Bounding Box (AABB)
class BoundingBox {
 public:
  // create square AABB around center edge length of 2*extends
  BoundingBox(glm::vec3 center, float extents);

  // True if sphere intersects with bounding box
  bool intersectsWithSphere(const glm::vec3 &spherePosition,
                            const float &sphereRadius);

 private:
  glm::vec3 center_;
  float extents_;
};
