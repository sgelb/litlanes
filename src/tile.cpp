#include "tile.h"

Tile::Tile(const int &x, const int &z,
           const std::shared_ptr<NoiseInterface> &noise,
           const GLuint &tileWidth)
    : noise_(noise),
      tileWidth_(tileWidth),
      xOffset_(x * Constants::TileWidth),
      zOffset_(z * Constants::TileWidth) {
  // initialize tile
  verticesCount_ = (tileWidth_ + 1) * (tileWidth_ + 1);
  quadtree_ = std::unique_ptr<Quadtree>(new Quadtree);
  possibleRiverSprings_.clear();
  createVertices();
  createTerrain();
  createRiver();
  createSea();
}

void Tile::setup() {
  // setup OpenGl stuff. there must be an opengl context!
  setupShader();
  setupBuffers();
}

// can we put this into ShaderManager?
void Tile::setupShader() {
  // Build and compile our shader program
  shader_ = std::unique_ptr<Shader>(
      new Shader("shader/default.vert", "shader/default.frag"));

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
  GLuint terrainVBO;  // Vertex Buffer Object
  glGenBuffers(1, &terrainVBO);
  glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
  glBufferData(GL_ARRAY_BUFFER, verticesCount_ * sizeof(Vertex),
               &vertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  GLuint terrainEBO;  // Element Buffer Object
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
  GLuint seaVBO;  // Vertex Buffer Object
  glGenBuffers(1, &seaVBO);
  glBindBuffer(GL_ARRAY_BUFFER, seaVBO);
  glBufferData(GL_ARRAY_BUFFER, seaVertices_.size() * sizeof(Vertex),
               &seaVertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  GLuint seaEBO;  // Element Buffer Object
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

void Tile::render(const glm::mat4 &view) {
  // Projection
  glm::mat4 projection = glm::perspective(
      Constants::Zoom, static_cast<GLfloat>(Constants::WindowWidth) /
                           static_cast<GLfloat>(Constants::WindowHeight),
      Constants::NearPlane, Constants::FarPlane);

  // Get the uniform locations
  GLint modelLoc = glGetUniformLocation(shader_->getProgram(), "model");
  GLint viewLoc = glGetUniformLocation(shader_->getProgram(), "view");
  GLint projLoc = glGetUniformLocation(shader_->getProgram(), "projection");

  // Pass the matrices to the shader
  glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // Calculate the model matrix and pass it to shader before drawing
  // TODO: what is happening here
  glm::mat4 model;
  model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  // Finally, draw tile elements
  glBindVertexArray(terrainVAO_);
  glDrawElements(GL_TRIANGLES, terrainIndices_.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);

  // Draw sea level
  glBindVertexArray(seaVAO_);
  glDrawElements(GL_TRIANGLES, seaIndices_.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Tile::cleanup() {
  // Properly de-allocate all resources once they've outlived their purpose
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
  possibleRiverSprings_.clear();

  // type of z and x is "signed int" instead of "unsigned size_t", so we don't
  // have to cast back to a signed type before calculating coordinates.
  for (int z = 0; z < width; z++) {
    for (int x = 0; x < width; x++) {
      idx = z * width + x;

      int worldX = xOffset_ + x;
      int worldZ = zOffset_ + z;

      // use world space coordinates of x and z to create
      // height generated with noise algorithm
      // result is float in [-1, 1], mapped to [0, Constants::MaxMeshHeight]
      y = (noise_->getValue(worldX, 0.0f, worldZ) + 1) / 2 * Constants::MaxMeshHeight;

      // remember idx if y >= Constants::MinimumHeightOfRiverSpring
      if (y >= Constants::MinimumHeightOfRiverSpring) {
        possibleRiverSprings_.push_back(idx);
      }

      // set position
      vertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(worldX),
                    static_cast<GLfloat>(y),
                    static_cast<GLfloat>(worldZ));

      // set color
      vertices_[idx].color = colorFromHeight(y);
    }
  }
}

void Tile::createTerrain() {
  terrainIndices_ = quadtree_->getIndicesOfLevel(Constants::MaximumLod);
}

void Tile::createRiver() {
  if (possibleRiverSprings_.empty()) {
    return;
  }

  // choose river spring from possibleRiverSprings_;
  auto engine = std::default_random_engine{};
  // always seed with same value. lucky number 9
  engine.seed(vertices_[9].position.y);
  std::shuffle(std::begin(possibleRiverSprings_),
      std::end(possibleRiverSprings_), engine);
  calculateRiverCourse(possibleRiverSprings_.front());
  for (GLuint riverIdx : riverIndices_) {
    vertices_[riverIdx].color = glm::vec3{0.0f, 0.5f, 1.0f};
  }
}

void Tile::calculateRiverCourse(const int &curIdx) {

  /* we look at neighboring vertices and add the lowest to river. what defines
   * as neigbour depends on the kind of grid and how the river is rendered.

    N--N--+   a neigbor of X shares an edge, so all Ns are neighbors of X.
    |\ |\ |
    | \| \|
    N--X--N
    |\ |\ |
    | \| \|
    +--N--N

  */

  // find lowest neighboring vertice
  float lowestHeight = vertices_[curIdx].position.y;
  int nextIdx = -1;

  int w = tileWidth_ + 1;
  std::vector<int> neighbors = {
    // row above
    static_cast<int>(curIdx - w - 1),
    static_cast<int>(curIdx - w),
    // same row
    curIdx - 1,
    curIdx + 1,
    // row below
    static_cast<int>(curIdx + w),
    static_cast<int>(curIdx + w + 1)
  };

  for (int neighbor : neighbors) {
    // low enough?
    if (vertices_[neighbor].position.y <= lowestHeight) {
      // on this tile?
      if (neighbor >= 0 && neighbor < verticesCount_) {
        lowestHeight = vertices_[neighbor].position.y;
        nextIdx = neighbor;
      }
    }
  }

  if (nextIdx < 0 || nextIdx >= verticesCount_) {
    // River ends if no neighbour is lower or is at tile border
    return;
  }

  // add to riverIndices_
  riverIndices_.push_back(static_cast<GLuint>(nextIdx));

  // call calculateRiverCourse(currentLocation)
  calculateRiverCourse(nextIdx);
}

void Tile::createSea() {
  // create seaVertices
  seaVertices_ = std::vector<Vertex>(verticesCount_);

  auto engine = std::default_random_engine{};
  engine.seed(vertices_[9].position.y);
  std::uniform_real_distribution<float> distribution(0.0, 0.3);

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

      // set position
      seaVertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(worldX),
                    static_cast<GLfloat>(seaLevel_ + distribution(engine)),
                    static_cast<GLfloat>(worldZ));

      // set color
      seaVertices_[idx].color = glm::vec3{0.0f, 0.5f, 1.0f};
    }
  }

  // create seaIndices
  seaIndices_ = terrainIndices_;
}


glm::vec3 Tile::colorFromHeight(const GLfloat &height) {
  // simplified color model with 5 "height zones"

  if (height > 0.9 * Constants::MaxMeshHeight) {
    // snow
    glm::vec3 color = {0.8f, 0.8f, 0.8f};
    return color;
  }
  if (height > 0.6 * Constants::MaxMeshHeight) {
    // rock
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    return color;
  }
  if (height > 0.15 * Constants::MaxMeshHeight) {
    // forest
    glm::vec3 color = {0.2f, 0.4f, 0.25f};
    return color;
  }
  // gras
  glm::vec3 color = {0.2f, 0.6f, 0.25f};
  return color;
}

std::vector<Vertex> Tile::getVertices() {
  return vertices_;
}

std::vector<GLuint> Tile::getIndices() {
  // used for testing
  return terrainIndices_;
}

void Tile::updateCoordinates(const int &x, const int &z) {
  xOffset_ = x * Constants::TileWidth;
  zOffset_ = z * Constants::TileWidth;
  createVertices();
  createTerrain();
  createRiver();
  createSea();
  setupBuffers();
}

void Tile::updateAlgorithm(const std::shared_ptr<NoiseInterface> noise) {
  noise_ = noise;
  createVertices();
  createTerrain();
  createRiver();
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
