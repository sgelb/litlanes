#pragma once

#include <glm/glm.hpp>

// Implements basic Axis-Aligned Bounding Box
class BoundingBox {
 public:
  BoundingBox(glm::vec3 position, float extents);
  bool intersectsWithSphere(const glm::vec3 &spherePosition,
      const float &sphereRadius);

 private:
  glm::vec3 position_;
  float extents_;
};
