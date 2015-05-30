#pragma once

#include <cmath>
#include <limits>
#include <glm/glm.hpp>
#include <vector>
#include <map>

#include "noise.h"


class WorleyNoise : public NoiseInterface {
  public:
    WorleyNoise();
    float getValue(const float &x, const float &y, const float &z);
    void initializeOptions();
    void setOptions(const NoiseOptions &options);

  private:
    std::vector<std::pair<float, float>> calculateFeaturePoints(const std::pair<float, float> &square);
    float nearestDistanceTo(const float &nearestDistance, const int &x, const int &z, const std::vector<std::pair<float, float>> &points);
};
