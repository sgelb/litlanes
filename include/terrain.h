#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>

#include <noise/noise.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/fast_square_root.hpp>  // fastNormalize
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "constants.h"
#include "quadtree.h"
#include "shader.h"


/**
 * @brief Vertex defined by position and color
 */
struct Vertex {
  glm::vec3 position;
  glm::vec3 color;
};


/**
 * @brief TODO: what is this class?
 */
class Terrain {
 public:
   /**
    * @brief Construct terrain tile of provided size
    *
    * @param tileWidth edge length of terrain tile
    */
  explicit Terrain(const int &x, const int &z, 
      const GLuint &tileWidth = Constants::TileWidth);
  /**
   * @brief Set up shader and opengl buffers
   */
  void setup();

  /**
   * @brief Set algorithm for heightmap generation
   *
   * @param algorithm TODO: define in Constants
   */
  void setAlgorithm(const int &algorithm);

  /**
   * @brief Update terrain before rendering
   */
  void update(const GLfloat &deltaTime);

  /**
   * @brief Render terrain
   *
   * @param view Viewing matrix for correct rendering
   */
  void render(const glm::mat4 &view);

  /**
   * @brief Clean up, i.e. delete buffer arrays
   */
  void cleanup();

  /**
   * @brief Vertices of terrain mesh
   *
   * @return vertices
   */
  std::vector<Vertex> getVertices();

  /**
   * @brief Indices of vertices of terrain mesh
   *
   * @return indices
   */
  std::vector<GLuint> getIndices();



 private:
  using modulePtr = std::unique_ptr<noise::module::Module>;

  GLuint tileWidth_;
  modulePtr noise_;
  std::unique_ptr<Shader> shader_;

  std::vector<Vertex> vertices_;
  std::vector<GLuint> indices_;
  std::unique_ptr<Quadtree> quadtree_;
  GLuint verticesCount_;
  GLuint VAO_;  // Vertex Array Object
  GLuint VBO_;  // Vertex Buffer Object
  GLuint EBO_;  // Element Buffer Object
  GLint objectColorLoc_;
  GLint lightColorLoc_;
  GLint lightPosLoc_;
  int xOffset_;
  int zOffset_;
  glm::vec3 lightPos_;

  void createVertices();
  void createIndices();
  void setupShader();
  void setupBuffers();

  void rotateLight();

  GLfloat mapToInterval(const GLfloat &input);
  glm::vec3 colorFromHeight(const GLfloat &height);
};
