#include <gtest/gtest.h>
#include <terrain.h>
#include <vector>

TEST(TerrainTest, verticesCount) {
  Terrain terrain(Constants::TileWidth);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  int expected = 6 * (Constants::TileWidth + 1) * (Constants::TileWidth + 1);
  int result = terrain.getVertices().size();
  ASSERT_EQ(expected, result);
}

TEST(TerrainTest, indicesCount) {
  Terrain terrain(Constants::TileWidth);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  // 2 triangles * 3 indices per tile * TileWidth^2 tiles
  int expected = 6 * Constants::TileWidth * Constants::TileWidth;
  int result = terrain.getIndices().size();
  ASSERT_EQ(expected, result);
}

TEST(TerrainTest, indicesPosition) {
  Terrain terrain(2);
  terrain.setAlgorithm(Constants::Perlin);
  terrain.create();
  std::vector<GLuint> expected = {0, 3, 4, 0, 4, 1, 1, 4, 5, 1, 5, 2,
                                  3, 6, 7, 3, 7, 4, 4, 7, 8, 4, 8, 5};
  std::vector<GLuint> indices = terrain.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

