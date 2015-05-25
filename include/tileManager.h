#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>
#include "tile.h"

class TileManager {
  public:
    void initialize(glm::vec3 const &currentPos_);
    void update(glm::vec3 const &currentPos_);
    void renderAll(const GLfloat &deltaTime, const glm::mat4 &viewMatrix);
    void cleanUp();
    void changeTileAlgorithm(const int &algorithm);

  private:
    glm::vec3 currentPos_;
    glm::vec3 previousPos_;
    std::vector<std::unique_ptr<Tile>> tiles_;

    void updatePosition();
    void updateTiles();
};
