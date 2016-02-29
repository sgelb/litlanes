/*
 * Copyright (C) 2016 sgelb
 *
 * This file is part of litlanes.
 *
 * litlanes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * litlanes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with litlanes.  If not, see <http://www.gnu.org/licenses/>.
 */

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
  // instead. Returns True if sphere intersects with bounding box
  return distance <= radius * radius;
}
