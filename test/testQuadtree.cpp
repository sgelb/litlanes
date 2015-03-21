#include <gtest/gtest.h>
#include <quadtree.h>
#include <constants.h>
#include <vector>

TEST(QuadtreeTest, verticesCountOfLowestLod) {
  Quadtree quadtree(0, 0);
  int count = quadtree.getIndices().size();
  EXPECT_EQ(6, count) << "Indices count differs " << count;
}

TEST(QuadtreeTest, lowestLodisNoLeaf) {
  Quadtree quadtree(0, 0);
  EXPECT_FALSE(quadtree.isLeaf());
}

TEST(QuadtreeTest, highestLodisLeaf) {
  Quadtree quadtree(Constants::MaximumLoD, 0);
  EXPECT_TRUE(quadtree.isLeaf());
}

TEST(QuadtreeTest, verticesPositionWithMaximumLoD) {
  Quadtree quadtree(Constants::MaximumLoD, 0);
  unsigned int w = Constants::TileWidth;
  std::vector<GLuint> expected = {0, w, w + 1, 0, w + 1, 1};
  std::vector<GLuint> indices = quadtree.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

TEST(QuadtreeTest, verticesPositionWithLoD1) {
  Quadtree quadtree(1, 0);
  unsigned int w = Constants::TileWidth * 2;
  std::vector<GLuint> expected = {0, w, w + 2, 0, w + 2, 2};
  std::vector<GLuint> indices = quadtree.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

TEST(QuadtreeTest, verticesPositionWithLoD0) {
  Quadtree quadtree(0, 0);
  unsigned int w = Constants::TileWidth * 4;
  std::vector<GLuint> expected = {0, w, w + 4, 0, w + 4, 4};
  std::vector<GLuint> indices = quadtree.getIndices();
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}
