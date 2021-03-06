#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <noise/noise.h>
#include <memory>
#include <iostream>
#include <limits>

#include "defaults.h"

struct NoiseOptions {
  float frequency;
  float lacunarity;
  int octaveCount;
  float persistence;
  int seed;
};

class NoiseInterface {
 public:
  virtual float getValue(const float &x, const float &y, const float &z) = 0;
  virtual void initializeOptions() = 0;
  virtual void setOptions(const NoiseOptions &options) = 0;

  NoiseOptions getOptions();

 protected:
  NoiseOptions options_;
  float applyResolution(const float &input);
};

class PerlinNoise : public NoiseInterface {
 public:
  PerlinNoise();
  float getValue(const float &x, const float &y, const float &z);
  void initializeOptions();
  void setOptions(const NoiseOptions &options);

 private:
  std::shared_ptr<noise::module::Perlin> noise_;
};

class RidgedMultiNoise : public NoiseInterface {
 public:
  RidgedMultiNoise();
  float getValue(const float &x, const float &y, const float &z);
  void initializeOptions();
  void setOptions(const NoiseOptions &options);

 private:
  std::shared_ptr<noise::module::RidgedMulti> noise_;
};

class BillowNoise : public NoiseInterface {
 public:
  BillowNoise();
  float getValue(const float &x, const float &y, const float &z);
  void initializeOptions();
  void setOptions(const NoiseOptions &options);

 private:
  std::shared_ptr<noise::module::Billow> noise_;
};

class RandomNoise : public NoiseInterface {
 public:
  float getValue(const float &x, const float &y, const float &z);
  void initializeOptions() {
  }
  void setOptions(const NoiseOptions &options) {
  }
};
