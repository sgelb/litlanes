#include "noiseManager.h"

void NoiseManager::setNoise(const int &algorithm) {
  /* const NoiseOptions &options) { */

  if (algorithm_ == algorithm) {
    return;
  }

  algorithm_ = algorithm;
  /* options_ = options; */

  switch (algorithm_) {
  case Constants::Perlin:
    noise_ = Noise<noise::module::Perlin>(
        std::make_shared<noise::module::Perlin>(new noise::module::Perlin));
    std::cout << "Perlin: " << noise_.noise->GetSeed() << std::endl;
    break;
  case Constants::RidgedMulti:
    noise_ = Noise<noise::module::RidgedMulti>(
        std::make_shared<noise::module::RidgedMulti>(
            new noise::module::RidgedMulti));
    std::cout << "Ridged: " << noise_.noise->GetSeed() << std::endl;
    break;
  default:
    std::cout << "Nada" << std::endl;
  }
}

/* template <class T> */
float NoiseManager::getValue(const float &x, const float &y, const float &z) {
  return noise_.GetValue(x, y, z);
}

void NoiseManager::setOptions(const NoiseOptions &options) {
  options_ = options;
}

NoiseOptions NoiseManager::getOptions() {
  return options_;
}

/* private: */
/*   T noise_; */
/*   int algorithm_; */
/*   noise::module::Perlin perlin_; */
/*   noise::module::RidgedMulti ridgedMulti_; */
/*   noise::module::Billow billow_; */
