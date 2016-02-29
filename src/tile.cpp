#include "tile.h"

Tile::Tile(const int &x, const int &z,
           const std::shared_ptr<NoiseInterface> &noise,
           const GLuint &tileWidth)
    : noise_(noise),
      tileWidth_(tileWidth),
      xOffset_(x * Defaults::TileWidth),
      zOffset_(z * Defaults::TileWidth),
      quadtree_(std::unique_ptr<Quadtree>(new Quadtree)),
      verticesCount_((tileWidth_ + 1) * (tileWidth_ + 1)) {
  // initialize tile
  createVertices();
  createTerrain();
  createSea();
}

void Tile::setup() {
  // setup OpenGl stuff. there must be an opengl context!
  setupShader();
  setupBuffers();
}

void Tile::setupShader() {
  // Build and compile our shader program
  shader_ = std::unique_ptr<Shader>(new Shader());
  shader_->load("shader/default.vert", GL_VERTEX_SHADER);
  shader_->load("shader/default.frag", GL_FRAGMENT_SHADER);
  shader_->use();
  lightPos_ = glm::vec3(500.0f, 500.0f, 0.0f);
  lightColorLoc_ = glGetUniformLocation(shader_->getProgram(), "lightColor");
  lightPosLoc_ = glGetUniformLocation(shader_->getProgram(), "lightPosition");
  glUniform3f(lightColorLoc_, 1.0f, 1.0f, 1.0f);
  glUniform3f(lightPosLoc_, lightPos_.x, lightPos_.y, lightPos_.z);
}

void Tile::setupBuffers() {
  setupTerrainBuffers();
  setupSeaBuffers();
}

// TODO: DRY for buffer setup
void Tile::setupTerrainBuffers() {
  // Bind VAO first,
  glGenVertexArrays(1, &terrainVAO_);
  glBindVertexArray(terrainVAO_);

  // then bind and set vertex buffers
  GLuint terrainVBO; // Vertex Buffer Object
  glGenBuffers(1, &terrainVBO);
  glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
  glBufferData(GL_ARRAY_BUFFER, verticesCount_ * sizeof(Vertex),
               &vertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  GLuint terrainEBO; // Element Buffer Object
  glGenBuffers(1, &terrainEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, terrainIndices_.size() * sizeof(GLuint),
               &terrainIndices_.front(), GL_STATIC_DRAW);

  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(0));

  // Color attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(offsetof(Vertex, color)));

  // Unbind buffers/arrays to prevent strange bugs
  glBindVertexArray(0);
}

void Tile::setupSeaBuffers() {
  // Bind VAO first,
  glGenVertexArrays(1, &seaVAO_);
  glBindVertexArray(seaVAO_);

  // then bind and set vertex buffers
  GLuint seaVBO; // Vertex Buffer Object
  glGenBuffers(1, &seaVBO);
  glBindBuffer(GL_ARRAY_BUFFER, seaVBO);
  glBufferData(GL_ARRAY_BUFFER, seaVertices_.size() * sizeof(Vertex),
               &seaVertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  GLuint seaEBO; // Element Buffer Object
  glGenBuffers(1, &seaEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, seaEBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, seaIndices_.size() * sizeof(GLuint),
               &seaIndices_.front(), GL_STATIC_DRAW);

  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(0));

  // Color attribute
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(offsetof(Vertex, color)));

  // Unbind buffers/arrays to prevent strange bugs
  glBindVertexArray(0);
}

void Tile::update(const GLfloat &deltaTime) {
  // right now, this just updates the light
  glm::mat4 rotationMat(1);
  rotationMat =
      glm::rotate(rotationMat, deltaTime * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
  lightPos_ = glm::vec3(rotationMat * glm::vec4(lightPos_, 1.0f));
  glUniform3f(lightPosLoc_, lightPos_.x, lightPos_.y, lightPos_.z);
}

void Tile::render(const glm::mat4 &viewMatrix) {
  // Projection
  glm::mat4 projection = glm::perspective(
      Defaults::Zoom, static_cast<GLfloat>(Defaults::WindowWidth) /
                          static_cast<GLfloat>(Defaults::WindowHeight),
      Defaults::NearPlane, Defaults::FarPlane);

  // Get the uniform locations
  GLint modelLoc = glGetUniformLocation(shader_->getProgram(), "model");
  GLint viewLoc = glGetUniformLocation(shader_->getProgram(), "view");
  GLint projLoc = glGetUniformLocation(shader_->getProgram(), "projection");

  // Pass the matrices to the shader
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Calculate the model matrix and pass it to shader before drawing
  glm::mat4 modelMatrix;
  modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.5f, 0.0f, -0.5f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

  // Finally, draw tile elements
  glBindVertexArray(terrainVAO_);
  glDrawElements(GL_TRIANGLES, terrainIndices_.size(), GL_UNSIGNED_INT,
                 nullptr);
  glBindVertexArray(0);

  // Draw sea level
  if (showSea_) {
    glBindVertexArray(seaVAO_);
    glDrawElements(GL_TRIANGLES, seaIndices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
  }
}

void Tile::cleanup() {
  glDeleteVertexArrays(1, &terrainVAO_);
  glDeleteVertexArrays(1, &seaVAO_);
}

void Tile::createVertices() {

  /*

   y ^ Right-handed coordinates system. y is height.
     |
     |
     +-----> x
    /
  z/

  the polygon mesh consists of vertices forming a square from (0, y, 0) to
  (width, y, width) with width*width subtiles. each subtile is split in two
  triangles from top left to bottom right.

  */

  // there are (tileWidth + 1)^2 vertices
  vertices_ = std::vector<Vertex>(verticesCount_);
  int idx = 0;
  size_t width = tileWidth_ + 1;
  GLfloat y;

  // type of z and x is "signed int" instead of "unsigned size_t", so we don't
  // have to cast back to a signed type before calculating coordinates.
  for (int z = 0; z < width; z++) {
    for (int x = 0; x < width; x++) {
      idx = z * width + x;

      int worldX = xOffset_ + x;
      int worldZ = zOffset_ + z;

      // use world space coordinates of x and z to create height generated with
      // noise algorithm
      y = (noise_->getValue(worldX, 0.0f, worldZ) + 1) / 2 *
          Defaults::MaxMeshHeight;

      // set position
      vertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(worldX), static_cast<GLfloat>(y),
                    static_cast<GLfloat>(worldZ));

      // set color
      vertices_[idx].color = colorFromHeight(y);
    }
  }
}

void Tile::createTerrain() {
  terrainIndices_ = quadtree_->getIndicesOfLevel(Defaults::MaximumLod);
}

void Tile::setShowSea(bool showSea) {
  showSea_ = showSea;
}

void Tile::createSea() {
  // create seaVertices
  seaVertices_ = std::vector<Vertex>(verticesCount_);

  int idx = 0;
  size_t width = tileWidth_ + 1;
  GLfloat y;

  // type of z and x is "signed int" instead of "unsigned size_t", so we don't
  // have to cast back to a signed type before calculating coordinates.
  for (int z = 0; z < width; z++) {
    for (int x = 0; x < width; x++) {
      idx = z * width + x;

      int worldX = xOffset_ + x;
      int worldZ = zOffset_ + z;

      // just to get some coherent height into it.
      float yOffset = 0.3 * std::sin(1.2 * worldX) * std::sin(1.3 * worldZ);

      // set position
      seaVertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(worldX),
                    static_cast<GLfloat>(seaLevel_ + yOffset),
                    static_cast<GLfloat>(worldZ));

      // set color
      seaVertices_[idx].color = glm::vec3{0.0f, 0.5f, 1.0f};
    }
  }

  // create seaIndices
  seaIndices_ = terrainIndices_;
}

glm::vec3 Tile::colorFromHeight(const GLfloat &height) {
  // very simple color model with 5 "height zones"

  if (height > 0.9 * Defaults::MaxMeshHeight) {
    // snow
    glm::vec3 color = {0.8f, 0.8f, 0.8f};
    return color;
  }
  if (height > 0.6 * Defaults::MaxMeshHeight) {
    // rock
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    return color;
  }
  if (height > 0.15 * Defaults::MaxMeshHeight) {
    // forest
    glm::vec3 color = {0.2f, 0.4f, 0.25f};
    return color;
  }
  // gras
  glm::vec3 color = {0.2f, 0.6f, 0.25f};
  return color;
}

std::vector<Vertex> Tile::getVertices() {
  // used for testing
  return vertices_;
}

std::vector<GLuint> Tile::getIndices() {
  // used for testing
  return terrainIndices_;
}

void Tile::updateCoordinates(const int &x, const int &z) {
  xOffset_ = x * Defaults::TileWidth;
  zOffset_ = z * Defaults::TileWidth;
  createVertices();
  createTerrain();
  createSea();
  setupBuffers();
}

void Tile::changeAlgorithm(const std::shared_ptr<NoiseInterface> noise) {
  noise_ = noise;
  createVertices();
  createTerrain();
  setupBuffers();
}

void Tile::setSeaLevel(const float &seaLevel) {
  seaLevel_ = seaLevel;
  createSea();
  setupSeaBuffers();
}

float Tile::getSeaLevel() {
  return seaLevel_;
}

float Tile::getHeightAtNeighborIndex(const int &curIdx,
                                     const int &neighborIdx) {
  // return height at neighborIdx

  if (neighborIdx >= 0 && neighborIdx < verticesCount_) {
    // neighborIdx is on same tile, just return height from vertices_
    return vertices_[neighborIdx].position.y;
  }

  // neighborIdx is on another tile, calculate coordinates and return height
  glm::vec3 coordinates = calculateCoordinates(curIdx, neighborIdx);
  return (noise_->getValue(coordinates.x, 0.0f, coordinates.z) + 1) / 2 *
         Defaults::MaxMeshHeight;
}

glm::vec3 Tile::calculateCoordinates(const int &curIdx,
                                     const int &neighborIdx) {
  float x = 0.0f;
  float z = 0.0f;
  int difference = neighborIdx - curIdx;

  if (difference < 1) {
    x = vertices_[curIdx].position.x + difference + tileWidth_ + 1;
    z = vertices_[curIdx].position.z - 1;
  }

  if (1 == abs(difference)) {
    x = vertices_[curIdx].position.x + difference;
    z = vertices_[curIdx].position.z;
  }

  if (difference > 1) {
    x = vertices_[curIdx].position.x + difference - tileWidth_ - 1;
    z = vertices_[curIdx].position.z + 1;
  }

  return glm::vec3(x, 0.0f, z);
}
