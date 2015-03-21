#include <gtest/gtest.h>
#include <terrain.h>
#include <vector>

TEST(TerrainTest, verticesCount) {
  Terrain terrain(4);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  int expected = 6 * 4 * 4; // 3 vectors + 3 colors * 4^2 vertices
  ASSERT_EQ(expected, terrain.getVertices().size());
}

TEST(TerrainTest, indicesCount) {
  Terrain terrain(4);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  int expected = 6 * 3 * 3; // 2 triangles * 3 indices per tile * (4-1)^2 tiles
  ASSERT_EQ(expected, terrain.getIndices().size());
}

TEST(TerrainTest, indicesPosition) {
  Terrain terrain(3);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  std::vector<GLuint> expected = {0, 3, 4, 0, 4, 1, 1, 4, 5, 1, 5, 2,
                                  3, 6, 7, 3, 7, 4, 4, 7, 8, 4, 8, 5};
  std::vector<GLuint> indices = terrain.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

