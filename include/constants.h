#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

/**
 * @brief Constants
 */
namespace Constants {
// WINDOW

//@{
/**
 * @brief Window size
 */
static const GLuint WindowWidth = 1200;
static const GLuint WindowHeight = 800;
//@}

// CAMERA

//@{
/**
 * @brief Distance to near/far plane of view frustum
 */
static const GLfloat NearPlane = 0.1f;
static const GLfloat FarPlane = 1000.0f;
//@}

//@{
/**
 * @brief Default camera values
 */
static const GLfloat Zoom = 45.0f;
static const GLfloat Yaw = -90.0f;
static const GLfloat Pitch = 0.0f;
static const GLfloat Speed = 60.0f;
static const GLfloat Sensitivity = 0.5f;
//@}

// TERRAIN

// noise algorithms
static const int Perlin = 0;
static const int RidgedMulti = 1;
static const int Billow = 2;
static const int Random = 3;
static const int Worley = 4;


// default width of terrain tile in triangles. Must be a factor of 2.
static const GLuint TileWidth = 64;

// max height of terrain mesh
static const GLfloat MaxMeshHeight = TileWidth / 4;

// minimum height of possible river springs
static const GLfloat MinimumHeightOfRiverSpring = MaxMeshHeight * 0.8;

// LEVEL OF DETAIL

static const int MaximumLod = log2(Constants::TileWidth);
/* static const std::vector<int> LodDistances = {800, 400, 100}; */

} // namespace Constants
