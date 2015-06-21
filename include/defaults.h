#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

/**
 * @brief Default values
 */
namespace Defaults {
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
static const GLfloat Pitch = -89.0f;
static const GLfloat Speed = 60.0f;
static const GLfloat Sensitivity = 0.5f;
//@}

// TERRAIN

// noise algorithms
static const int Perlin = 0;
static const int RidgedMulti = 1;
static const int Billow = 2;
static const int Random = 3;

// default width of terrain tile in triangles. Must be a factor of 2.
static const GLuint TileWidth = 64;

// default resolution
static const GLuint Resolution  = 64;

// max height of terrain mesh
static const GLfloat MaxMeshHeight = Resolution / 2;

// minimum height of possible river springs
static const GLfloat MinimumHeightOfRiverSpring = MaxMeshHeight * 0.9;

// RIVER
static const int MaximumRiverLength = 100;
static const int RiversPerTile = 1;

// LEVEL OF DETAIL

static const int MaximumLod = log2(Defaults::TileWidth);

} // namespace Constants
