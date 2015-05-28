#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <noise/noise.h>
#include <iostream>
#include "constants.h"

/**
 * @brief Options of libnoise algorithms
 */
struct NoiseOptions {
  float frequency;
  float lacunarity;
  int octaveCount;
  float persistence;
  int seed;
};

class Noise {
 public:
  explicit Noise(std::shared_ptr<T> t) : noise(t) {
  }
  std::shared_ptr<T> noise;
};

/* template <class T> */
class NoiseManager {
  using modulePtr = std::shared_ptr<noise::module::Module>;

 public:
  /* NoiseManager(T noise); */
  void setNoise(const int &algorithm); //, const NoiseOptions &options);

  float getValue(const float &x, const float &y, const float &z);

  void setOptions(const NoiseOptions &options);
  NoiseOptions getOptions();

 private:
  noise_ = nullptr;
  NoiseOptions options_;
  int algorithm_;
};
