#include "terrain.h"

// TODO: create Vertice class and divide position and color
// TODO: write tests

Terrain::Terrain(const GLuint meshSize) : meshSize_(meshSize) {
}

void Terrain::setAlgorithm(int algorithm) {
  switch (algorithm) {
    case Constants::Perlin:
      this->noise_ = std::unique_ptr<noise::module::Module>(
          new noise::module::Perlin);
      break;
    case Constants::RidgedMulti:
      this->noise_ = std::unique_ptr<noise::module::Module>(
          new noise::module::RidgedMulti);
      break;
    default:
      break;
  }
}

void Terrain::create() {
  this->createVertices();
  this->createIndices();
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

  this->vertices_ = std::vector<GLfloat>();

  for (size_t z = 0; z < this->meshSize_; z++) {
    for (size_t x = 0; x < this->meshSize_; x++) {
      // use x and z (mapped to [-1, 1]) to create height generated with
      // noise algorithm
      GLfloat y = this->noise_->GetValue(mapToInterval(x), 0.5f,
          mapToInterval(z));

      // coordinates
      this->vertices_.push_back(static_cast<GLfloat>(x));
      this->vertices_.push_back(
          static_cast<GLfloat>(Constants::MaxMeshHeight * y));
      this->vertices_.push_back(static_cast<GLfloat>(z));

      // color
      std::vector<GLfloat> color = colorFromHeight(y);
      this->vertices_.insert(this->vertices_.end(), color.begin(), color.end());
    }
  }
}


void Terrain::createIndices() {
  this->indices_ = std::vector<GLuint>();
  for (size_t z = 0; z < this->meshSize_ - 1; z++) {
    for (size_t x = 0; x < this->meshSize_ - 1; x++) {
      /*

      +---x
      |
      |   TL_TR  We split each subtile in two triangles and put their indices
      y   |\  |  counterclockwise in indices-Array:
          | \ |  Left triangle:  TL->BL->BR
          |__\|  Right triangle: TL->BR->TR
          BL BR

      */

      GLuint tl = x + this->meshSize_ * z;
      GLuint tr = tl + 1;
      GLuint bl = tl + this->meshSize_;
      GLuint br = bl + 1;

      // left triangle
      this->indices_.push_back(tl);
      this->indices_.push_back(bl);
      this->indices_.push_back(br);

      // right triangle
      this->indices_.push_back(tl);
      this->indices_.push_back(br);
      this->indices_.push_back(tr);
    }
  }
}


std::vector<GLfloat> Terrain::colorFromHeight(GLfloat height) {
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
  return this->indices_;
}

std::vector<GLfloat> Terrain::getVertices() {
  return this->vertices_;
}

GLfloat Terrain::mapToInterval(const GLfloat &input) {
  // map input from [0, Constants::MeshWidth] to [-1, 1]
  return 2 * (input / Constants::MeshWidth) - 1;
}
