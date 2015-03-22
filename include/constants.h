#pragma once

#include <vector>

namespace Constants {
// WINDOW

// default width and height of window
static const GLuint WindowWidth = 1200;
static const GLuint WindowHeight = 800;

// CAMERA

// distance to near/far plane of vire frutrum
static const GLfloat NearPlane = 0.1f;
static const GLfloat FarPlane = 1000.0f;

// default camera aspect/zoom
static const GLfloat Zoom = 45.0f;
static const GLfloat Yaw = -90.0f;
static const GLfloat Pitch = 0.0f;
static const GLfloat Speed = 30.0f;
static const GLfloat Sensitivty = 0.5f;

// TERRAIN

// noise algorithms
static const int Perlin = 1;
static const int RidgedMulti = 2;

// default width of terrain tile in triangles. Must be a factor of 2.
static const GLuint TileWidth = 256;

// max height of terrain mesh
static const GLfloat MaxMeshHeight = TileWidth / 5;

// LEVEL OF DETAIL

static const int MaximumLod = log2(Constants::TileWidth);
/* static const std::vector<int> LodDistances = {800, 400, 100}; */

} // namespace Constants
