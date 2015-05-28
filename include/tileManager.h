#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "tile.h"

class TileManager {
  using modulePtr = std::shared_ptr<noise::module::Module>;

 public:
  void initialize(glm::vec3 const &currentPos_);
  void update(glm::vec3 const &currentPos_);
  void renderAll(const GLfloat &deltaTime, const glm::mat4 &viewMatrix);
  void cleanUp();
  void changeTileAlgorithm(const int &algorithm);
  void setOctaveCount(const int &octaves);

 private:
  modulePtr noise_;
  int algorithm_;
  glm::vec3 currentPos_;
  glm::vec3 previousPos_;
  std::vector<std::unique_ptr<Tile>> tiles_;

  void setNoise(const int &algorithm);
  void updatePosition();
  void updateTiles();
};
