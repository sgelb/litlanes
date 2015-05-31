#include "noise.h"

NoiseOptions NoiseInterface::getOptions() {
  return options_;
}

float NoiseInterface::mapToInterval(const float &input) {
  // FIXME: input can be negative, so we have to map from [INT_MIN, INT_MAX]
  // map input from [0, Constants::TileWidth] to [-1, 1]
  /* return input / std::numeric_limits<int>::max(); */
  return 2 * (input / Constants::TileWidth) - 1;
}


// Perlin

PerlinNoise::PerlinNoise() {
  noise_ = std::shared_ptr<noise::module::Perlin>(new noise::module::Perlin);
  initializeOptions();
}

void PerlinNoise::initializeOptions() {
  options_.frequency = noise::module::DEFAULT_PERLIN_FREQUENCY;
  options_.lacunarity = noise::module::DEFAULT_PERLIN_LACUNARITY;
  options_.octaveCount = noise::module::DEFAULT_PERLIN_OCTAVE_COUNT;
  options_.persistence = noise::module::DEFAULT_PERLIN_PERSISTENCE;
  options_.seed = noise::module::DEFAULT_PERLIN_SEED;
}

void PerlinNoise::setOptions(const NoiseOptions &options) {
  options_ = options;
  noise_->SetFrequency(options_.frequency);
  noise_->SetLacunarity(options_.lacunarity);
  noise_->SetOctaveCount(options_.octaveCount);
  noise_->SetPersistence(options_.persistence);
  noise_->SetSeed(options_.seed);
}

GLfloat PerlinNoise::getValue(const float &x, const float &y, const float &z) {
  return noise_->GetValue(mapToInterval(x), y, mapToInterval(z));
}


// RidgedMulti

RidgedMultiNoise::RidgedMultiNoise() {
  noise_ = std::shared_ptr<noise::module::RidgedMulti>(
      new noise::module::RidgedMulti);
  initializeOptions();
}

void RidgedMultiNoise::initializeOptions() {
  options_.frequency = noise::module::DEFAULT_RIDGED_FREQUENCY;
  options_.lacunarity = noise::module::DEFAULT_RIDGED_LACUNARITY;
  options_.octaveCount = noise::module::DEFAULT_RIDGED_OCTAVE_COUNT;
  options_.persistence = 0;
  options_.seed = noise::module::DEFAULT_RIDGED_SEED;
}

GLfloat RidgedMultiNoise::getValue(const float &x, const float &y,
                                   const float &z) {
  return noise_->GetValue(mapToInterval(x), y, mapToInterval(z));
}

void RidgedMultiNoise::setOptions(const NoiseOptions &options) {
  options_ = options;
  noise_->SetFrequency(options_.frequency);
  noise_->SetLacunarity(options_.lacunarity);
  noise_->SetOctaveCount(options_.octaveCount);
  noise_->SetSeed(options_.seed);
}


// Voronoi

// TODO: libnoise/src/module/voronoi.cpp: use nth closes point
VoronoiNoise::VoronoiNoise() {
  noise_ = std::shared_ptr<noise::module::Voronoi>(new noise::module::Voronoi);
  noise_->EnableDistance(true);
}

GLfloat VoronoiNoise::getValue(const float &x, const float &y, const float &z) {
  return noise_->GetValue(mapToInterval(x), y, mapToInterval(z));
}

void VoronoiNoise::initializeOptions() {
  options_.frequency = 0.0f;
  options_.lacunarity = 0;
  options_.octaveCount = 0;
  options_.persistence = 0;
  options_.seed = noise::module::DEFAULT_VORONOI_SEED;
}

void VoronoiNoise::setOptions(const NoiseOptions &options) {
  noise_->SetFrequency(options.frequency);
  /* noise_->SetDisplacement(options.frequency); */
};


// Random

float RandomNoise::getValue(const float &x, const float &y, const float &z) {
  // return random float between 0 and 1
  return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}


