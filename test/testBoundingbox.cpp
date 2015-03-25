#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "boundingbox.h"

TEST(BoundingboxTest, sphereShouldNotIntersect) {
  glm::vec3 position(3.0f, 3.0f, 3.0f);
  float extent = 0.5;
  BoundingBox bb(position, extent);  // x,y,z: 2-4

  glm::vec3 sphere(1.0f, 1.0f, 1.0f);
  float radius = 1.0f;
  EXPECT_FALSE(bb.intersectsWithSphere(sphere, radius));
}

TEST(BoundingboxTest, sphereShouldIntersect) {
  glm::vec3 position(3.0f, 3.0f, 3.0f);
  float extent = 0.5;
  BoundingBox bb(position, extent);  // x,y,z: 2-4

  glm::vec3 sphere(1.0f, 1.0f, 1.0f);
  float radius = 3.0f;
  EXPECT_TRUE(bb.intersectsWithSphere(sphere, radius));
}

TEST(BoundingboxTest, sphereShouldIntersectIfInsideBox) {
  glm::vec3 position(1.0f, 1.0f, 1.0f);
  float extent = 3.0;
  BoundingBox bb(position, extent);  // x,y,z: 2-4

  glm::vec3 sphere(1.0f, 1.0f, 1.0f);
  float radius = 1.0f;
  EXPECT_TRUE(bb.intersectsWithSphere(sphere, radius));
}

TEST(BoundingboxTest, sphereShouldIntersectIfBoxInside) {
  glm::vec3 position(1.0f, 1.0f, 1.0f);
  float extent = 1.0;
  BoundingBox bb(position, extent);  // x,y,z: 2-4

  glm::vec3 sphere(1.0f, 1.0f, 1.0f);
  float radius = 3.0f;
  EXPECT_TRUE(bb.intersectsWithSphere(sphere, radius));
}
