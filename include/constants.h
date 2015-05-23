#pragma once

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
static const GLfloat Sensitivty = 0.5f;
//@}

// TERRAIN

// noise algorithms
static const int Perlin = 0;
static const int RidgedMulti = 1;
static const int DiamondSquare = 2;
static const int Billow = 3;
static const int Random = 5;

// default width of terrain tile in triangles. Must be a factor of 2.
static const GLuint TileWidth = 64;

// max height of terrain mesh
static const GLfloat MaxMeshHeight = TileWidth / 5;

// LEVEL OF DETAIL

static const int MaximumLod = log2(Constants::TileWidth);
/* static const std::vector<int> LodDistances = {800, 400, 100}; */

} // namespace Constants
