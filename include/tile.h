#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <noise/noise.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp> // fastNormalize
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"
#include "quadtree.h"
#include "shader.h"
#include "noise.h"

/**
 * @brief Vertex defined by position and color
 */
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
};

/**
 * @brief TODO
 */
class Tile {
 public:
  /**
   * @brief Construct tile tile of provided size
   *
   * @param tileWidth edge length of tile tile
   */
  explicit Tile(const int &x, const int &z,
                const std::shared_ptr<NoiseInterface> &noise =
                    std::shared_ptr<NoiseInterface>(new PerlinNoise),
                const GLuint &tileWidth = Constants::TileWidth);
  /**
   * @brief Set up shader and opengl buffers
   */
  void setup();

  /**
   * @brief Update tile before rendering
   */
  void update(const GLfloat &deltaTime);

  /**
   * @brief Render tile
   *
   * @param view Viewing matrix for correct rendering
   */
  void render(const glm::mat4 &view);

  /**
   * @brief Clean up, i.e. delete buffer arrays
   */
  void cleanup();

  /**
   * @brief Update coordinates and recalculate values
   *
   * @param x
   * @param z
   */
  void updateCoordinates(const int &x, const int &z);

  /**
   * @brief Vertices of tile mesh
   *
   * @return vertices
   */
  std::vector<Vertex> getVertices();

  /**
   * @brief Indices of vertices of tile mesh
   *
   * @return indices
   */
  std::vector<GLuint> getIndices();

  /**
   * @brief Set new algorithm and update vertices
   *
   */
  void updateAlgorithm(const std::shared_ptr<NoiseInterface> noise);

 private:
  GLuint tileWidth_;
  std::shared_ptr<NoiseInterface> noise_;
  std::unique_ptr<Shader> shader_;

  std::vector<Vertex> vertices_;
  std::vector<GLuint> indices_;
  std::unique_ptr<Quadtree> quadtree_;
  GLuint verticesCount_;
  GLuint VAO_; // Vertex Array Object
  GLuint VBO_; // Vertex Buffer Object
  GLuint EBO_; // Element Buffer Object
  GLint objectColorLoc_;
  GLint lightColorLoc_;
  GLint lightPosLoc_;
  int xOffset_;
  int zOffset_;
  glm::vec3 lightPos_;

  std::vector<int> possibleRiverSprings_;
  std::vector<GLuint> riverCourse_;
  int remainingRiverCapacity_;

  void createVertices();
  void createIndices();
  void createRiver();
  void calculateRiverCourse(const int &curLoc);
  void setupShader();
  void setupBuffers();

  void rotateLight();

  glm::vec3 colorFromHeight(const GLfloat &height);
};
