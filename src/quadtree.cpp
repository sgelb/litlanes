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

#include "quadtree.h"

Quadtree::Quadtree(const int &level, const int &startpoint)
    : level_(level), startpoint_(startpoint) {
  // A tree data structure in which each node has exactly four children. Used to
  // recursivly partition a tile in different levels of details.

  // Offset/Distance between vertices in this level. Offset on lod 0 is
  // TileWidth and decreases with every iteration. highest level of detail has
  // offset of 1.
  int offset = pow(2, Defaults::MaximumLod - level_);

  /*

  +---x
  |
  |   TL_TR  We split each quad in two triangles and put their six indices
  y   |\  |  counterclockwise in array indices_:
      | \ |  Left triangle:  TL->BL->BR
      |__\|  Right triangle: TL->BR->TR
      BL BR

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

  // add children until we reached MaximumLod
  if (level_ < Defaults::MaximumLod) {
    this->isLeaf_ = false;

    // calculate starting points of children
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
  // a quadtree without children is a leaf
  return isLeaf_;
}

std::vector<GLuint> Quadtree::getIndicesOfLevel(const int &lod) {
  // return indices for specified LOD
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
