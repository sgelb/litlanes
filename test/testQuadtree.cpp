#include <gtest/gtest.h>
#include <quadtree.h>
#include <constants.h>
#include <vector>

TEST(QuadtreeTest, indicesCountOfLowestLod) {
  Quadtree quadtree(0, 0);
  int expected = 6;
  int result = quadtree.getIndicesOfLevel(0).size();
  EXPECT_EQ(expected, result) << "Indices count differ: " << result;
}

TEST(QuadtreeTest, indicesCountOfMaximumLod) {
  Quadtree quadtree(0, 0);
  int expected = 6*Constants::TileWidth*Constants::TileWidth;
  int result = quadtree.getIndicesOfLevel(Constants::MaximumLod).size();
  EXPECT_EQ(expected, result) << "Indices count differ: " << result;
}

TEST(QuadtreeTest, indicesPositionOfMinimumLod) {
  Quadtree quadtree(0, 0);
  unsigned int w = Constants::TileWidth;
  std::vector<GLuint> expected = {0, w*w, w*w + w, 0, w*w + w, w};
  std::vector<GLuint> indices = quadtree.getIndicesOfLevel(0);
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

TEST(QuadtreeTest, indicesPositionOfLod1) {
  Quadtree quadtree(0, 0);
  unsigned int w = Constants::TileWidth;
  std::vector<GLuint> expected = {
    0, w*w/2, w*w/2 + w/2, 0, w*w/2 + w/2, w/2,  // nw
    w*w/2, w*w, w*w + w/2, w*w/2, w*w + w/2, w*w/2 + w/2,  // sw
    w*w/2 + w/2, w*w + w/2, w*w + w, w*w/2 + w/2, w*w + w, w*w/2 + w,  // se
    w/2, w*w/2 + w/2, w*w/2 + w, w/2, w*w/2 + w, w};  // nw
  std::vector<GLuint> indices = quadtree.getIndicesOfLevel(1);
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}


TEST(QuadtreeTest, lowestLodisNoLeaf) {
  Quadtree quadtree(0, 0);
  EXPECT_FALSE(quadtree.isLeaf());
}

TEST(QuadtreeTest, maximumLodIsLeaf) {
  Quadtree quadtree(Constants::MaximumLod, 0);
  EXPECT_TRUE(quadtree.isLeaf());
}
