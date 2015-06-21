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

#include "defaults.h"
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
                const GLuint &tileWidth = Defaults::TileWidth);
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
   * @brief Change noise algorithm
   *
   * @param noise
   */
  void updateAlgorithm(const std::shared_ptr<NoiseInterface> noise);

  /**
   * @brief Set sea level
   *
   * @param seaLevel
   */
  void setSeaLevel(const float &seaLevel);

  /**
   * @brief Get sea level
   *
   * @return sea level
   */
  float getSeaLevel();

  /**
   * @brief Set visibility of sea
   *
   * @param bool showSea
   */
  void setShowSea(bool showSea);

 private:
  GLuint tileWidth_;
  std::shared_ptr<NoiseInterface> noise_;
  std::unique_ptr<Shader> shader_;
  std::unique_ptr<Quadtree> quadtree_;

  std::vector<Vertex> vertices_;
  GLuint verticesCount_;

  GLuint terrainVAO_; // Vertex Array Object
  std::vector<GLuint> terrainIndices_;
  std::vector<GLuint> riverIndices_;

  GLuint seaVAO_; // Vertex Array Object
  std::vector<Vertex> seaVertices_;
  std::vector<GLuint> seaIndices_;
  float seaLevel_;
  bool showSea_;

  GLint objectColorLoc_;
  GLint lightColorLoc_;
  GLint lightPosLoc_;
  int xOffset_;
  int zOffset_;
  glm::vec3 lightPos_;

  void createVertices();
  void createTerrain();
  void createSea();

  void calculateRiverCourse(const int &curLoc, const int &remainingLength);
  void setupShader();

  void setupBuffers();
  void setupTerrainBuffers();
  void setupSeaBuffers();

  void rotateLight();

  glm::vec3 colorFromHeight(const GLfloat &height);
  float getHeightAtNeighborIndex(const int &curIdx, const int &neighborIdx);
  glm::vec3 calculateCoordinates(const int &curIdx, const int &neighborIdx);
};
