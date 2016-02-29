#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>

namespace Defaults {
// WINDOW

// Window size
static const GLuint WindowWidth = 1200;
static const GLuint WindowHeight = 800;


// TERRAIN

// noise algorithms
static const int Perlin = 0;
static const int RidgedMulti = 1;
static const int Billow = 2;
static const int Random = 3;

// Default width of terrain tile in triangles. Must be a factor of 2.
static const GLuint TileWidth = 64;

// Default resolution
static const GLuint Resolution = 64;

// Maximum height of terrain
static const GLfloat MaxMeshHeight = Resolution / 2;

// CAMERA

// Distance to near/far plane of view frustum
static const GLfloat NearPlane = 0.1f;
static const GLfloat FarPlane = 1000.0f;

// Default camera values
static const GLfloat Zoom = 45.0f;
static const GLfloat Yaw = -90.0f;
static const GLfloat Pitch = -89.0f;
static const GLfloat Speed = 60.0f;
static const GLfloat Sensitivity = 0.5f;
static const glm::vec3 CameraPosition =
    glm::vec3(3 * Defaults::TileWidth / 2, 0.0f, 3 * Defaults::TileWidth / 2);

// LEVEL OF DETAIL

static const int MaximumLod = log2(Defaults::TileWidth);

} // namespace Constants
