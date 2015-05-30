#include "worley.h"

WorleyNoise::WorleyNoise() {
}

float WorleyNoise::getValue(const float &worldX, const float &y, const float &worldZ) {
  // Each square between "int corners" contains a number of feature points
  std::map<std::pair<float, float>, std::vector<std::pair<float, float>>> featurePoints;

  // Calculate coordinates squares surrounding (x,z)
  int upperX = 1;
  int upperZ = 1;
  if (std::floor(worldX) != worldX && std::floor(worldZ) != worldZ) {
    // x and z are not whole numbers and we have to look at nine squares
    upperX = 2;
    upperZ = 2;
  } 

  float nearestDistance = std::numeric_limits<float>::max();

  // For every square:
  for (int x = -1; x < upperX; x++) {
    for (int z = -1; z < upperZ; z++) {
      // calculate coordinates of feature points (cache them)
      std::pair<float, float> square = std::make_pair(worldX + x, worldZ + z);
      /* glm::vec2 square = glm::vec2(worldX + x, worldZ + z); */
      // TODO: cache featurePoints[square]
      featurePoints[square] = calculateFeaturePoints(square);
      nearestDistance = nearestDistanceTo(nearestDistance, x, z, featurePoints[square]);
    }
  }

  // Return distance to nearest feature point
  return nearestDistance;
}

std::vector<std::pair<float, float>> WorleyNoise::calculateFeaturePoints(const std::pair<float, float> &square) {
  std::vector<std::pair<float, float>> points;
  points.push_back(std::make_pair(1.0f, 0.5f));
  return points;
}

float WorleyNoise::nearestDistanceTo(const float &nearestDistance, const int
    &x, const int &z, const std::vector<std::pair<float, float>> &points) {
  // TODO: implement other distance functions
  // Euclidian distance. Because we only _compare_ the results, we can skip the
  // expensive calculation of square root
  float distance = std::numeric_limits<float>::max();
  for (auto point : points) {
    float d = std::abs(x - point.first) * std::abs(z - point.second);
    distance = (distance < d) ? distance : d;
  }

  // return smaller distance
  return (distance < nearestDistance) ? distance : nearestDistance;
}

// overwrite
void WorleyNoise::initializeOptions() {}
void WorleyNoise::setOptions(const NoiseOptions &options) {}
