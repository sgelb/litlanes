#include "quadtree.h"

Quadtree::Quadtree(const int &level, const int &startpoint)
    : level_(level), startpoint_(startpoint) {

  // offset of vertices in this level. root is level 0 and
  // highest level of detail has offset of 1
  int offset = pow(2, Defaults::MaximumLod - level_);

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
  int bl = tl + (Defaults::TileWidth + 1) * offset;
  int br = bl + offset;

  // left triangle
  indices_[0] = tl;
  indices_[1] = bl;
  indices_[2] = br;

  // right triangle
  indices_[3] = tl;
  indices_[4] = br;
  indices_[5] = tr;

  // create bounding box
  boundingBox_ = std::unique_ptr<BoundingBox>(
      new BoundingBox(glm::vec3(static_cast<float>((tr - tl)) / 2.0f,
                                0.0f,
                                static_cast<float>((bl - tl)) / 2.0f),
                      static_cast<float>(offset) / 2.0f));

  // add children
  if (level_ < Defaults::MaximumLod) {
    this->isLeaf_ = false;

    // calculate starting points
    int tlChild = tl;
    int trChild = tl + offset / 2;
    int blChild = tl + (Defaults::TileWidth + 1) * offset / 2;
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

bool Quadtree::isLeaf() {
  return isLeaf_;
}

std::vector<GLuint> Quadtree::getIndicesOfLevel(const int &lod) {
  if (isLeaf_ || level_ == lod) {
    return indices_;
  }

  std::vector<GLuint> indices;
  for (auto child : children_) {
    auto i = child->getIndicesOfLevel(lod);
    indices.insert(indices.cend(), i.cbegin(), i.cend());
  }

  return indices;
}
