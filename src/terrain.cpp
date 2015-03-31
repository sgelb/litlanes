#include "terrain.h"

Terrain::Terrain(const GLuint &tileWidth)
    : tileWidth_(tileWidth), noise_(modulePtr(new noise::module::Perlin)) {
  verticesCount_ = (tileWidth_ + 1) * (tileWidth_ + 1);
  quadtree_ = std::unique_ptr<Quadtree>(new Quadtree);
  createVertices();
  createIndices();
  createNormals();
  setup();
}

Terrain::~Terrain() {
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO_);
  glDeleteBuffers(1, &VBO_);
}

void Terrain::setup() {
  // Build and compile our shader program
  // TODO: proper loading of shaders
  shader_ = std::unique_ptr<Shader>(
      new Shader("shader/default.vert", "shader/default.frag"));
  // Activate shader
  shader_->use();

  glGenVertexArrays(1, &VAO_);
  glGenBuffers(1, &VBO_);
  glGenBuffers(1, &EBO_);

  // Bind VAO first,
  glBindVertexArray(VAO_);

  // then bind and set vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO_);
  glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex),
               &vertices_.front(), GL_STATIC_DRAW);

  // the element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint),
               &indices_.front(), GL_STATIC_DRAW);

  // Position attribute
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(0));
  // Normal attribute
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<GLvoid *>(offsetof(Vertex, normal)));

  // Unbind buffers/arrays to prevent strange bugs
  glBindVertexArray(0);
}

void Terrain::update() {
  // do we need to create new tiles?
  // recalc indices/lod
}

void Terrain::render(const glm::mat4 &view) {
  // Render
  // Clear the colorbuffer
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Projection
  glm::mat4 projection;
  projection = glm::perspective(
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
  glm::mat4 model;
  model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  // Finally, draw terrain elements
  glBindVertexArray(VAO_);
  glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
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
      vertices_[idx].position =
          glm::vec3(static_cast<GLfloat>(x),
                    static_cast<GLfloat>(Constants::MaxMeshHeight * y),
                    static_cast<GLfloat>(z));

      // set default normal
      vertices_[idx].normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }
  }
}

void Terrain::createIndices() {
  indices_ = quadtree_->getIndicesOfLevel(Constants::MaximumLod);
}

// TODO: move to quadtree because it depends on lod?
void Terrain::createNormals() {
  // create normal of every triangle for calculation of vertice normals
  for (size_t idx = 0; idx < indices_.size(); idx += 3) {
    // vertices v0, v1 and v2 form a triangle
    // calculate the normal of this triangle:
    size_t v0 = indices_[idx];
    size_t v1 = indices_[idx + 1];
    size_t v2 = indices_[idx + 2];
    glm::vec3 normal =
        glm::cross(vertices_[v1].position - vertices_[v0].position,
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

void Terrain::setAlgorithm(const int &algorithm) {
  switch (algorithm) {
  case Constants::Perlin:
    noise_ = modulePtr(new noise::module::Perlin);
    break;
  case Constants::RidgedMulti:
    noise_ = modulePtr(new noise::module::RidgedMulti);
    break;
  default:
    noise_ = modulePtr(new noise::module::Perlin);
  }
}

std::vector<Vertex> Terrain::getVertices() {
  return vertices_;
}

std::vector<GLuint> Terrain::getIndices() {
  /* return quadtree_->getIndicesOfLevel(Constants::MaximumLod); */
  return indices_;
}

GLfloat Terrain::mapToInterval(const GLfloat &input) {
  // map input from [0, Constants::TileWidth] to [-1, 1]
  return 2 * (input / Constants::TileWidth) - 1;
}
