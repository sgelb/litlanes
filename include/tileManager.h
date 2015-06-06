#pragma once

#include <memory>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "tile.h"

#include "noise.h"

class TileManager {

 public:
  void initialize(glm::vec3 const &currentPos_);
  void update(glm::vec3 const &currentPos_);
  void renderAll(const GLfloat &deltaTime, const glm::mat4 &viewMatrix);
  void cleanUp();
  void setTileAlgorithm(const int &algorithm);
  NoiseOptions getOptions();
  void setTileAlgorithmOptions(const NoiseOptions &options);
  float getSeaLevel();
  void setSeaLevel(const float &seaLevel);

 private:
  int currentAlgorithm_;
  std::map<int, std::shared_ptr<NoiseInterface>> noiseCache_;
  std::shared_ptr<NoiseInterface> noise_;
  glm::vec3 currentPos_;
  glm::vec3 previousPos_;
  std::vector<std::unique_ptr<Tile>> tiles_;
  float seaLevel_;

  void setNoise(const int &algorithm);
  void updatePosition();
  void updateTiles();
};
