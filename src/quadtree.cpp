#include "quadtree.h"

Quadtree::Quadtree(const int &level, const int &startpoint)
    : level_(level), startpoint_(startpoint) {
  // TODO: better documentation
  // offset of vertices in this level. highest level has maximum
  // resolution aka offset = 1
  // level 0:  offset = 2^MaxLod = 4
  // level 1:  offset = 2^MaxLod = 2
  // level 2:  offset = 2^MaxLod = 1
  int offset = pow(2, Constants::MaximumLoD - level_);

  /*

  +---x
  |
  |   tl_tr  We split each quad in two triangles and put their indices
  y   |\  |  counterclockwise in indices-Array:
      | \ |  Left triangle:  TL->BL->BR
      |__\|  Right triangle: TL->BR->TR
      bl br

  */

  indices_ = std::vector<GLuint>(6);

  int tl = startpoint;
  int tr = startpoint + offset;
  int bl = tl + Constants::TileWidth * offset;
  int br = bl + offset;

  // left triangle
  indices_[0] = tl;
  indices_[1] = bl;
  indices_[2] = br;

  // right triangle
  indices_[3] = tl;
  indices_[4] = br;
  indices_[5] = tr;

  // add children
  if (level_ < Constants::MaximumLoD) {
    this->isLeaf_ = false;

    // calculate starting points
    int tlChild = tl;
    int trChild = tlChild + offset / 2;
    int blChild = tlChild + Constants::TileWidth * offset / 2;
    int brChild = blChild + offset / 2;

    // add children counterclockwise
    children_ = std::vector<std::shared_ptr<Quadtree>>(4);
    int nextLevel = level_ + 1;

    children_[0] = std::shared_ptr<Quadtree>(new Quadtree(nextLevel, tlChild));
    children_[1] = std::shared_ptr<Quadtree>(new Quadtree(nextLevel, blChild));
    children_[2] = std::shared_ptr<Quadtree>(new Quadtree(nextLevel, brChild));
    children_[3] = std::shared_ptr<Quadtree>(new Quadtree(nextLevel, trChild));

  } else {
    isLeaf_ = true;
  }
}

std::vector<GLuint> Quadtree::getIndices() {
  return indices_;
}

bool Quadtree::isLeaf() {
  return isLeaf_;
}
