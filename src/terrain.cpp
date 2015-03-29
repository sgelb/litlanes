#include "terrain.h"

Terrain::Terrain(const GLuint tileWidth)
    : tileWidth_(tileWidth),
      noise_(modulePtr(new noise::module::Perlin)) {
}

void Terrain::setAlgorithm(const int &algorithm) {
  switch (algorithm) {
  case Constants::Perlin:
    noise_ = modulePtr(new noise::module::Perlin);
    break;
  case Constants::RidgedMulti:
    noise_ =
        modulePtr(new noise::module::RidgedMulti);
    break;
  default:
    noise_ = modulePtr(new noise::module::Perlin);
  }
}

void Terrain::create() {
  verticesCount_ = (tileWidth_ + 1) * (tileWidth_ + 1);
  createHeightMap();
  createIndices(); // TODO: remove
  createNormals();
}

void Terrain::createHeightMap() {

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

  // (tileWidth + 1)^2 vertices
  vertices_ = std::vector<Vertex>(verticesCount_);
  int idx = 0;
  size_t width = tileWidth_ + 1;
  GLfloat y;

  for (size_t z = 0; z < width; z++) {
    for (size_t x = 0; x < width; x++) {
      idx = z * width + x;

      // use x and z (mapped to [-1, 1]) to create height generated with
      // noise algorithm
      y = noise_->GetValue(mapToInterval(x), 0.0f, mapToInterval(z));

      // set position
      vertices_[idx].position = glm::vec3(
          static_cast<GLfloat>(x),
          static_cast<GLfloat>(Constants::MaxMeshHeight * y),
          static_cast<GLfloat>(z)
          );

      // set default normal
      vertices_[idx].normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
  }
}

// TODO: move to quadtree?
void Terrain::createNormals() {
  // create normal of every triangle for calculation of vertice normals
  for (size_t idx = 0; idx < indices_.size(); idx += 3) {
    // vertices v0, v1 and v2 form a triangle
    // calculate the normal of this triangle:
    size_t v0 = indices_[idx];
    size_t v1 = indices_[idx+1];
    size_t v2 = indices_[idx+2];
    glm::vec3 normal = glm::cross(
        vertices_[v1].position - vertices_[v0].position,
        vertices_[v2].position - vertices_[v0].position);

    // add normal to each vertice
    vertices_[v0].normal += normal;
    vertices_[v1].normal += normal;
    vertices_[v2].normal += normal;
  }

  for (auto vertex : vertices_) {
    // fastNormalize() is faster but normnalize() is more accurate
    /* vertex.normal = glm::normalize(vertex.normal); */
    vertex.normal = glm::fastNormalize(vertex.normal);
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

  // number of indices: 2 triangles * 3 indices per tile * (tileWidth)^2 tiles
  indices_ = std::vector<GLuint>(6 * tileWidth_ * tileWidth_);
  int idx = 0;

  for (size_t z = 0; z < tileWidth_; z++) {
    for (size_t x = 0; x < tileWidth_; x++) {
      GLuint tl = x + (tileWidth_ + 1) * z;
      GLuint tr = tl + 1;
      GLuint bl = tl + tileWidth_ + 1;
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

std::vector<Vertex> Terrain::getVertices() {
  return vertices_;
}

GLfloat Terrain::mapToInterval(const GLfloat &input) {
  // map input from [0, Constants::TileWidth] to [-1, 1]
  return 2 * (input / Constants::TileWidth) - 1;
}
