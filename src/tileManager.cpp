#include "tileManager.h"

#include "noiseManager.h"

void TileManager::initialize(const glm::vec3 &currentPos) {
  currentPos_ = currentPos;
  previousPos_ = currentPos;

  algorithm_ = Constants::Perlin;
  noise_ = modulePtr(new noise::module::Perlin);

  // create first nine tiles, from (0,0) to (2,2)
  //
  // order of for-loops matters
  // +----- x
  // |0 1 2
  // |3 4 5
  // |6 7 8
  // z

  for (int z = 0; z < 3; z++) {
    for (int x = 0; x < 3; x++) {
      std::unique_ptr<Tile> tile(new Tile(x, z, noise_));
      tile->setup();
      tiles_.push_back(std::move(tile));
    }
  }
}

void TileManager::update(glm::vec3 const &currentPos_) {
  int currentTileX = std::floor(currentPos_.x / Constants::TileWidth);
  int currentTileZ = std::floor(currentPos_.z / Constants::TileWidth);
  int diffX = currentTileX - std::floor(previousPos_.x / Constants::TileWidth);
  int diffZ = currentTileZ - std::floor(previousPos_.z / Constants::TileWidth);

  previousPos_ = currentPos_;

  // order of tiles in std::shared_ptr<Tile> tile_:
  // +----- x
  // |0 1 2
  // |3 4 5
  // |6 7 8
  // z
  //
  // tile coordinates relative to middle tile:
  // ------+------+-----
  // -1,-1 | 0,-1 | 1,-1   z = -1
  // ------+------+-----
  // -1,0  | 0,0  | 1,0    z = 0
  // ------+------+-----
  // -1,1  | 0,1  | 1,1    z = 1
  // ------+------+-----
  // x = -1 x = 0  x = 1

  if (diffX == 0 && diffZ == 0) {
    return; // we are still in middle tile, nothing to do
  }

  // Moving -Z or North
  if (diffZ < 0) {
    // Move first two rows down
    // 0 1 2      6 7 8
    // 3 4 5  ->  0 1 2
    // 6 7 8      3 4 5
    std::rotate(tiles_.begin(), tiles_.begin() + 6, tiles_.end());

    // Update first row
    tiles_[0]->updateCoordinates(currentTileX - 1, currentTileZ - 1);
    tiles_[1]->updateCoordinates(currentTileX, currentTileZ - 1);
    tiles_[2]->updateCoordinates(currentTileX + 1, currentTileZ - 1);
  }

  // Moving Z or South
  if (diffZ > 0) {
    // Move last two rows up
    // 0 1 2      3 4 5
    // 3 4 5  ->  6 7 8
    // 6 7 8      0 1 2
    std::rotate(tiles_.begin(), tiles_.begin() + 3, tiles_.end());

    // Update last row
    tiles_[6]->updateCoordinates(currentTileX - 1, currentTileZ + 1);
    tiles_[7]->updateCoordinates(currentTileX, currentTileZ + 1);
    tiles_[8]->updateCoordinates(currentTileX + 1, currentTileZ + 1);
  }

  // Moving -X or West
  if (diffX < 0) {
    // Move first two columns right
    // 0 1 2      2 0 1
    // 3 4 5  ->  5 3 4
    // 6 7 8      8 6 7
    std::rotate(tiles_.begin(), tiles_.begin() + 2, tiles_.begin() + 3);
    std::rotate(tiles_.begin() + 3, tiles_.begin() + 5, tiles_.begin() + 6);
    std::rotate(tiles_.begin() + 6, tiles_.begin() + 8, tiles_.end());

    // Update first column
    tiles_[0]->updateCoordinates(currentTileX - 1, currentTileZ - 1);
    tiles_[3]->updateCoordinates(currentTileX - 1, currentTileZ);
    tiles_[6]->updateCoordinates(currentTileX - 1, currentTileZ + 1);
  }

  // Moving X or East
  if (diffX > 0) {
    // Move last two columns left
    // 0 1 2      1 2 0
    // 3 4 5  ->  4 5 3
    // 6 7 8      7 8 6
    std::rotate(tiles_.begin(), tiles_.begin() + 1, tiles_.begin() + 3);
    std::rotate(tiles_.begin() + 3, tiles_.begin() + 4, tiles_.begin() + 6);
    std::rotate(tiles_.begin() + 6, tiles_.begin() + 7, tiles_.end());

    // Update last column
    tiles_[2]->updateCoordinates(currentTileX + 1, currentTileZ - 1);
    tiles_[5]->updateCoordinates(currentTileX + 1, currentTileZ);
    tiles_[8]->updateCoordinates(currentTileX + 1, currentTileZ + 1);
  }
}

void TileManager::renderAll(const GLfloat &deltaTime,
                            const glm::mat4 &viewMatrix) {
  // Update and render tiles
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->update(deltaTime);
    tiles_[idx]->render(viewMatrix);
  }
}

void TileManager::cleanUp() {
  // Clean up tile
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->cleanup();
  }
}

void TileManager::setOctaveCount(const int &octaves) {
  // TODO: this is ugly!
  switch (algorithm_) {
  case Constants::Perlin:
    static_cast<noise::module::Perlin&>(*noise_).SetOctaveCount(octaves);
    changeTileAlgorithm(algorithm_);
    break;
  case Constants::RidgedMulti:
    /* noise_ = modulePtr(new noise::module::RidgedMulti); */
    break;
  case Constants::Billow:
    /* noise_ = modulePtr(new noise::module::Billow); */
    break;
  default:
    break;
    /* noise_ = modulePtr(new noise::module::Perlin); */
  }
}

void TileManager::setNoise(const int &algorithm) {
  if (algorithm_ == algorithm) {
    return;
  }
  algorithm_ = algorithm;

  // TODO: this is ugly!
  switch (algorithm_) {
  case Constants::Perlin:
    noise_ = modulePtr(new noise::module::Perlin);
    break;
  case Constants::RidgedMulti:
    noise_ = modulePtr(new noise::module::RidgedMulti);
    break;
  case Constants::Billow:
    noise_ = modulePtr(new noise::module::Billow);
    break;
  default:
    noise_ = modulePtr(new noise::module::Perlin);
  }
}

void TileManager::changeTileAlgorithm(const int &algorithm) {
  setNoise(algorithm);

  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->updateAlgorithm(noise_);
  }
}
