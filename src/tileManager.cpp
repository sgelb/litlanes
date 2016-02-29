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

#include "tileManager.h"

void TileManager::initialize(const glm::vec3 &currentPos) {
  currentPos_ = currentPos;
  previousPos_ = currentPos;

  // default algorithm for terrain generation is PerlinNoise
  currentAlgorithm_ = Defaults::Perlin;
  noise_ = std::shared_ptr<NoiseInterface>(new PerlinNoise);
  // add to cache
  noiseCache_[Defaults::Perlin] = noise_;
  seaLevel_ = Defaults::MaxMeshHeight / 5;
  showSea_ = true;

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
      tile->setSeaLevel(seaLevel_);
      tile->setShowSea(showSea_);
      tiles_.push_back(std::move(tile));
    }
  }
}

void TileManager::update(glm::vec3 const &currentPos_) {
  int currentTileX = std::floor(currentPos_.x / Defaults::TileWidth);
  int currentTileZ = std::floor(currentPos_.z / Defaults::TileWidth);
  int diffX = currentTileX - std::floor(previousPos_.x / Defaults::TileWidth);
  int diffZ = currentTileZ - std::floor(previousPos_.z / Defaults::TileWidth);

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
    // right now, this just updates the light
    tiles_[idx]->update(deltaTime);
    tiles_[idx]->render(viewMatrix);
  }
}

void TileManager::cleanUp() {
  // Clean up tiles
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->cleanup();
  }
}

void TileManager::setTileAlgorithm(const int &algorithm) {
  if (currentAlgorithm_ == algorithm) {
    return;
  }
  currentAlgorithm_ = algorithm;

  // try to use exisiting noise instance from noiseCache, otherwise create and
  // add new one
  auto result = noiseCache_.find(algorithm);
  if (result != noiseCache_.end()) {
    // use already existing noise from cache
    noise_ = result->second;
  } else {
    // create new noise
    switch (algorithm) {
    case Defaults::Perlin:
      noise_ = std::shared_ptr<NoiseInterface>(new PerlinNoise);
      break;
    case Defaults::RidgedMulti:
      noise_ = std::shared_ptr<NoiseInterface>(new RidgedMultiNoise);
      break;
    case Defaults::Billow:
      noise_ = std::shared_ptr<NoiseInterface>(new BillowNoise);
      break;
    case Defaults::Random:
      noise_ = std::shared_ptr<NoiseInterface>(new RandomNoise);
      break;
    default:
      std::cerr << "Error: unknown algorithm " << algorithm << std::endl;
    }
    // and add to cache
    noiseCache_[algorithm] = noise_;
  }

  // update tiles
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->changeAlgorithm(noise_);
  }
}

NoiseOptions TileManager::getOptions() {
  return noise_->getOptions();
}

void TileManager::setTileAlgorithmOptions(const NoiseOptions &options) {
  noise_->setOptions(options);
  // update tiles
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->changeAlgorithm(noise_);
  }
}

bool TileManager::getShowSea() {
  return showSea_;
}

void TileManager::setShowSea(bool showSea) {
  showSea_ = showSea;
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->setShowSea(showSea_);
  }
}

float TileManager::getSeaLevel() {
  return seaLevel_;
}

void TileManager::setSeaLevel(const float &seaLevel) {
  seaLevel_ = seaLevel;
  for (size_t idx = 0; idx < tiles_.size(); idx++) {
    tiles_[idx]->setSeaLevel(seaLevel_);
  }
}
