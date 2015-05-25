#include <gtest/gtest.h>
#include <tile.h>
#include <vector>

TEST(TileTest, verticesCount) {
  Tile tile(0, 0);
  int expected = (Constants::TileWidth + 1) * (Constants::TileWidth + 1);
  int result = tile.getVertices().size();
  ASSERT_EQ(expected, result);
}

TEST(TileTest, indicesCount) {
  Tile tile(0, 0);
  // 2 triangles * 3 indices per tile * TileWidth^2 tiles
  int expected = 6 * Constants::TileWidth * Constants::TileWidth;
  int result = tile.getIndices().size();
  ASSERT_EQ(expected, result);
}

TEST(TileTest, indicesFirstTriangle) {
  Tile tile(0, 0);
  std::vector<GLuint> expected = {0, Constants::TileWidth + 1,
    Constants::TileWidth + 2};
  std::vector<GLuint> indices = tile.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}
