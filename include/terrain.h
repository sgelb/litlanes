#pragma once

// GLFW
#include <GLFW/glfw3.h>

// libnoise
#include <noise/noise.h>

#include <constants.h>

#include <vector>
#include <iostream>

class Terrain {
  public:
    explicit Terrain(GLuint meshSize = Constants::MeshWidth);
    std::vector<GLfloat> getVertices();
    std::vector<GLuint> getIndices();

  private:
    GLuint meshSize;
    void createVertices();
    void createIndices();
    std::vector<GLfloat> vertices;
    std::vector<GLuint> indices;
    GLfloat mapToInterval(const GLfloat &input);
};

