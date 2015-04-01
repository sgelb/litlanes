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


struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
};


class Terrain {
 public:
  explicit Terrain(const GLuint &tileWidth = Constants::TileWidth);
  ~Terrain();
  void setAlgorithm(const int &algorithm);
  void update();
  void render(const glm::mat4 &view);
  std::vector<Vertex> getVertices();
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
  glm::vec3 lightPos_;

  void createVertices();
  void createIndices();
  void createNormals();
  void setupShader();
  void setupBuffers();

  GLfloat mapToInterval(const GLfloat &input);
  glm::vec3 colorFromHeight(const GLfloat &height);
};
