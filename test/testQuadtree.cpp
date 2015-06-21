#include <gtest/gtest.h>
#include <quadtree.h>
#include <defaults.h>
#include <vector>

TEST(QuadtreeTest, indicesCountOfLowestLod) {
  Quadtree quadtree;
  int expected = 6;
  int result = quadtree.getIndicesOfLevel(0).size();
  EXPECT_EQ(expected, result) << "Indices count differ: " << result;
}

TEST(QuadtreeTest, indicesCountOfMaximumLod) {
  Quadtree quadtree;
  int expected = 6*Defaults::TileWidth*Defaults::TileWidth;
  int result = quadtree.getIndicesOfLevel(Defaults::MaximumLod).size();
  EXPECT_EQ(expected, result) << "Indices count differ: " << result;
}

TEST(QuadtreeTest, indicesPositionOfMinimumLod) {
  Quadtree quadtree;
  unsigned int w = Defaults::TileWidth;
  std::vector<GLuint> expected = {0, w*(w+1), w*(w+1) + w, 0, w*(w+1) + w, w};
  std::vector<GLuint> indices = quadtree.getIndicesOfLevel(0);
  for (size_t i = 0; i < expected.size(); i++) {
    EXPECT_EQ(expected[i], indices[i]) << "Vectors differ at index " << i;
  }
}

TEST(QuadtreeTest, indicesPositionOfLod1) {
  Quadtree quadtree;
  unsigned int w = Defaults::TileWidth;
  std::vector<GLuint> expected = {
    // nw
    0, w*(w+1)/2, w*(w+1)/2 + w/2, 0, w*(w+1)/2 + w/2, w/2,
    // sw
    w*(w+1)/2, w*(w+1), w*(w+1) + w/2, w*(w+1)/2, w*(w+1) + w/2,
    w*(w+1)/2 + w/2,
    // se
    w*(w+1)/2 + w/2, w*(w+1) + w/2, w*(w+1) + w, w*(w+1)/2 + w/2, w*(w+1) + w,
    w*(w+1)/2 + w,
    // nw
    w/2, w*(w+1)/2 + w/2, w*(w+1)/2 + w, w/2, w*(w+1)/2 + w, w};
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
  Quadtree quadtree(Defaults::MaximumLod, 0);
  EXPECT_TRUE(quadtree.isLeaf());
}
