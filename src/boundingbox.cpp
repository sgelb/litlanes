#include "boundingbox.h"

BoundingBox::BoundingBox(glm::vec3 center, float extents)
    : center_(center), extents_(extents) {
}

bool BoundingBox::intersectsWithSphere(const glm::vec3 &sphere,
                                       const float &radius) {
  // use euclidian distance from box to sphere to detect intersecting

  float distance = 0;
  float s = 0;

  for (int axis = 0; axis < 3; axis++) {

    // if sphere is "below" box
    if (sphere[axis] < (center_[axis] - extents_)) {
      s = sphere[axis] - (center_[axis] - extents_);
      distance += s * s;
    }

    // if sphere is "above" box
    if (sphere[axis] > (center_[axis] + extents_)) {
      s = sphere[axis] - (center_[axis] + extents_);
      distance += s * s;
    }

    // if sphere center is inside of box, add nothing to distance
  }

  // avoid expensive calculation of square root of distance by squaring radius
  return distance <= radius * radius;
}
