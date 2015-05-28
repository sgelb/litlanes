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
  createVertices();
  createIndices();
  run_ = true;
}

void Tile::setup() {
  // setup OpenGl stuff. there must be an opengl context!
  setupShader();
  setupBuffers();
}

void Tile::setupShader() {
  // Build and compile our shader program
  // TODO: proper loading of shaders
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
  // Bind VAO first,
  glGenVertexArrays(1, &VAO_);
  glBindVertexArray(VAO_);

  // then bind and set vertex buffers
  glGenBuffers(1, &VBO_);
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  glGenBuffers(1, &EBO_);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint),
               &indices_.front(), GL_STATIC_DRAW);

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
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
  glBindVertexArray(0);
}

void Tile::cleanup() {
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
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

      // use world space coordinates of x and z (mapped to [-1, 1]) to create
      // height generated with noise algorithm
      // y can be used as a seed
      y = noise_->getValue(mapToInterval(worldX), 0.0f, mapToInterval(worldZ));

      // set position
      vertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(worldX),
                    static_cast<GLfloat>(Constants::MaxMeshHeight * y),
                    static_cast<GLfloat>(worldZ));

      // set color
      vertices_[idx].color = colorFromHeight(y);
    }
  }
}

void Tile::createIndices() {
  indices_ = quadtree_->getIndicesOfLevel(Constants::MaximumLod);
}

glm::vec3 Tile::colorFromHeight(const GLfloat &height) {
  // simplified color model with 5 "height zones"

  if (height > 0.9) {
    // snow
    glm::vec3 color = {0.8f, 0.8f, 0.8f};
    return color;
  }
  if (height > 0.3) {
    // rock
    glm::vec3 color = {0.5f, 0.5f, 0.5f};
    return color;
  }
  if (height > -0.5) {
    // forest
    glm::vec3 color = {0.2f, 0.4f, 0.25f};
    return color;
  }
  if (height > -0.55) {
    // beach
    glm::vec3 color = {0.65f, 0.65f, 0.0f};
    return color;
  }
  // water
  glm::vec3 color = {0.0f, 0.5f, 1.0f};
  return color;
}

std::vector<Vertex> Tile::getVertices() {
  return vertices_;
}

std::vector<GLuint> Tile::getIndices() {
  /* return quadtree_->getIndicesOfLevel(Constants::MaximumLod); */
  return indices_;
}

GLfloat Tile::mapToInterval(const GLfloat &input) {
  // FIXME: input can be negative, so we have to map from [INT_MIN, INT_MAX]
  // map input from [0, Constants::TileWidth] to [-1, 1]
  return 2 * (input / Constants::TileWidth) - 1;
}

void Tile::updateCoordinates(const int &x, const int &z) {
  xOffset_ = x * Constants::TileWidth;
  zOffset_ = z * Constants::TileWidth;
  createVertices();
  createIndices();
  setupBuffers();
}

void Tile::updateAlgorithm(const std::shared_ptr<NoiseInterface> noise) {
  noise_ = noise;
  createVertices();
  createIndices();
  setupBuffers();
}
