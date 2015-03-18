#include "terrain.h"

// TODO: create Vertice class and divide position and color
// TODO: write tests

Terrain::Terrain(const GLuint meshSize)
    : meshSize_(meshSize),
      noise_(
          std::unique_ptr<noise::module::Module>(new noise::module::Perlin)) {
}

void Terrain::setAlgorithm(const int &algorithm) {
  switch (algorithm) {
  case Constants::Perlin:
    noise_ = std::unique_ptr<noise::module::Module>(new noise::module::Perlin);
    break;
  case Constants::RidgedMulti:
    noise_ =
        std::unique_ptr<noise::module::Module>(new noise::module::RidgedMulti);
    break;
  default:
    noise_ = std::unique_ptr<noise::module::Module>(new noise::module::Perlin);
  }
}

void Terrain::create() {
  createVertices();
  createIndices();
}

void Terrain::createVertices() {

  /*

   y ^ Right-handed coordinates system. y is height.
     |
     |
     +-----> x
    /
  z/

  the polygon mesh consists of vertices forming a large square from (0, y, 0) to
  (width, y, width) with width*width subtiles. each subtile is split in two
  triangles from top left to bottom right.

  */

  // number of vertices: (3 vectors + 3 colors) * meshSize^2 vertices
  vertices_ = std::vector<GLfloat>(6 * meshSize_ * meshSize_);
  int idx = 0;
  GLfloat y;

  for (size_t z = 0; z < meshSize_; z++) {
    for (size_t x = 0; x < meshSize_; x++) {
      // use x and z (mapped to [-1, 1]) to create height generated with
      // noise algorithm
      y = noise_->GetValue(mapToInterval(x), 0.0f, mapToInterval(z));

      // coordinates
      vertices_[idx++] = static_cast<GLfloat>(x);
      vertices_[idx++] = static_cast<GLfloat>(Constants::MaxMeshHeight * y);
      vertices_[idx++] = static_cast<GLfloat>(z);

      // color
      for (auto color : colorFromHeight(y)) {
        vertices_[idx++] = color;
      }
    }
  }
}

void Terrain::createIndices() {

  /*

  +---x
  |
  |   tl_tr  We split each subtile in two triangles and put their indices
  y   |\  |  counterclockwise in indices-Array:
      | \ |  Left triangle:  TL->BL->BR
      |__\|  Right triangle: TL->BR->TR
      bl br

  */

  // number of indices: 2 triangles * 3 indices per tile * (meshSize-1)^2 tiles
  indices_ = std::vector<GLuint>(6 * (meshSize_ - 1) * (meshSize_ - 1));
  int idx = 0;

  for (size_t z = 0; z < meshSize_ - 1; z++) {
    for (size_t x = 0; x < meshSize_ - 1; x++) {
      GLuint tl = x + meshSize_ * z;
      GLuint tr = tl + 1;
      GLuint bl = tl + meshSize_;
      GLuint br = bl + 1;

      // left triangle
      indices_[idx++] = tl;
      indices_[idx++] = bl;
      indices_[idx++] = br;

      // right triangle
      indices_[idx++] = tl;
      indices_[idx++] = br;
      indices_[idx++] = tr;
    }
  }
}

std::vector<GLfloat> Terrain::colorFromHeight(const GLfloat &height) {
  // simplified color model with 5 "height zones"

  if (height > 0.9) {
    // snow
    std::vector<GLfloat> color = {1.0f, 1.0f, 1.0f};
    return color;
  }
  if (height > 0.3) {
    // rock
    std::vector<GLfloat> color = {0.5f, 0.5f, 0.5f};
    return color;
  }
  if (height > -0.5) {
    // forest
    std::vector<GLfloat> color = {0.2f, 0.4f, 0.25f};
    return color;
  }
  if (height > -0.55) {
    // beach
    std::vector<GLfloat> color = {0.65f, 0.65f, 0.0f};
    return color;
  }
  // water
  std::vector<GLfloat> color = {0.0f, 0.5f, 1.0f};
  return color;
}

std::vector<GLuint> Terrain::getIndices() {
  return indices_;
}

std::vector<GLfloat> Terrain::getVertices() {
  return vertices_;
}

GLfloat Terrain::mapToInterval(const GLfloat &input) {
  // map input from [0, Constants::MeshWidth] to [-1, 1]
  return 2 * (input / Constants::MeshWidth) - 1;
}
