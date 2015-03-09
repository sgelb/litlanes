#pragma once

namespace Constants {
// WINDOW

// default width and height of window
static const GLuint WindowWidth = 800;
static const GLuint WindowHeight = 600;

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

// default width of terrain mesh
static const GLuint MeshWidth = 128;

// max height of terrain mesh
static const GLfloat MaxMeshHeight = 50.0f;

}; // namespace Constants
