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

// Vertex defined by position and color
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
};

class Tile {
 public:
  explicit Tile(const int &x, const int &z,
                const std::shared_ptr<NoiseInterface> &noise =
                    std::shared_ptr<NoiseInterface>(new PerlinNoise),
                const GLuint &tileWidth = Defaults::TileWidth);
  void setup();
  void update(const GLfloat &deltaTime);
  void render(const glm::mat4 &viewMatrix);
  void cleanup();
  void updateCoordinates(const int &x, const int &z);
  void changeAlgorithm(const std::shared_ptr<NoiseInterface> noise);
  void setSeaLevel(const float &seaLevel);
  float getSeaLevel();
  void setShowSea(bool showSea);

  // for testing
  std::vector<GLuint> getIndices();
  std::vector<Vertex> getVertices();

 private:
  std::shared_ptr<NoiseInterface> noise_;
  GLuint tileWidth_;
  int xOffset_;
  int zOffset_;
  std::unique_ptr<Quadtree> quadtree_;
  GLuint verticesCount_;

  std::unique_ptr<Shader> shader_;
  std::vector<Vertex> vertices_;

  GLuint terrainVAO_; // Vertex Array Object
  std::vector<GLuint> terrainIndices_;

  GLuint seaVAO_; // Vertex Array Object
  std::vector<Vertex> seaVertices_;
  std::vector<GLuint> seaIndices_;
  float seaLevel_;
  bool showSea_;

  GLint objectColorLoc_;
  GLint lightColorLoc_;
  GLint lightPosLoc_;
  glm::vec3 lightPos_;

  void createVertices();
  void createTerrain();
  void createSea();

  void setupShader();

  void setupBuffers();
  void setupTerrainBuffers();
  void setupSeaBuffers();

  void rotateLight();

  glm::vec3 colorFromHeight(const GLfloat &height);
  float getHeightAtNeighborIndex(const int &curIdx, const int &neighborIdx);
  glm::vec3 calculateCoordinates(const int &curIdx, const int &neighborIdx);
};
