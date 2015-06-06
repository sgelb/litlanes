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

  void setSeaLevel(const float &seaLevel);
  float getSeaLevel();

 private:
  GLuint tileWidth_;
  std::shared_ptr<NoiseInterface> noise_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<Quadtree> quadtree_;

  std::vector<Vertex> vertices_;
  GLuint verticesCount_;

  GLuint terrainVAO_; // Vertex Array Object
  std::vector<GLuint> terrainIndices_;

  GLuint riverVAO_; // Vertex Array Object
  std::vector<GLuint> riverIndices_;

  GLuint seaVAO_; // Vertex Array Object
  std::vector<Vertex> seaVertices_;
  std::vector<GLuint> seaIndices_;
  float seaLevel_;

  GLint objectColorLoc_;
  GLint lightColorLoc_;
  GLint lightPosLoc_;
  int xOffset_;
  int zOffset_;
  glm::vec3 lightPos_;

  std::vector<int> possibleRiverSprings_;
  int remainingRiverCapacity_;

  void createVertices();
  void createTerrain();
  void createRiver();
  void createSea();

  void calculateRiverCourse(const int &curLoc);
  void setupShader();

  void setupBuffers();
  void setupTerrainBuffers();
  void setupRiverBuffers();
  void setupSeaBuffers();

  void rotateLight();

  glm::vec3 colorFromHeight(const GLfloat &height);
};
