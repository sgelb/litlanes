#include "boundingbox.h"

BoundingBox::BoundingBox(glm::vec3 position, float extents)
  : position_(position), extents_(extents) {
}

bool BoundingBox::intersectsWithSphere(const glm::vec3 &sphere,
    const float &radius) {

  float distance = 0;
  float s = 0;

  // find distance from box to sphere
  for (int axis = 0; axis < 3; axis++) {

    // if sphere is "above" box
    if (sphere[axis] < (position_[axis] - extents_)) {
      s = sphere[axis] - (position_[axis] - extents_);
      distance += s*s;
    }

    // if sphere is "below" box
    if (sphere[axis] > (position_[axis] + extents_)) {
      s = sphere[axis] - (position_[axis] + extents_);
      distance += s*s;
    }
  }

  return distance <= radius*radius;
}
