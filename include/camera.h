#pragma once

// Std. Includes
#include <vector>
#include <iostream>

// GL Includes
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <constants.h>

// An abstract camera class that processes input and calculates the
// corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Camera {
 public:
  // Defines several possible options for camera movement. Used as
  // abstraction to stay away from window-system specific input methods
  enum CameraMovement { Forward, Backward, Left, Right };

  // Constructor with vectors
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = Constants::Yaw, GLfloat pitch = Constants::Pitch);

  // Constructor with scalar values
  Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX, GLfloat upY,
         GLfloat upZ, GLfloat yaw, GLfloat pitch);

  // Returns the view matrix calculated using Eular Angles and the LookAt Matrix
  glm::mat4 getViewMatrix();

  // Processes input received from any keyboard-like input system. Accepts input
  // parameter in the form of camera defined ENUM (to abstract it from windowing
  // systems)
  void processKeyboard(CameraMovement direction, GLfloat deltaTime);

  // Processes input received from a mouse input system. Expects the offset
  // value in both the x and y direction.
  void processMouseMovement(GLfloat xoffset, GLfloat yoffset,
                            GLboolean constrainPitch = true);

  // Processes input received from a mouse scroll-wheel event. Only requires
  // input on the vertical wheel-axis
  void processMouseScroll(GLfloat yoffset);

  GLfloat getZoom();

 private:
  // Camera Attributes
  glm::vec3 position;
  glm::vec3 front;
  glm::vec3 up;
  glm::vec3 right;
  glm::vec3 worldUp;

  // Eular Angles
  GLfloat yaw;
  GLfloat pitch;

  // Camera options
  GLfloat movementSpeed;
  GLfloat mouseSensitivity;
  GLfloat zoom;

  // Calculates the front vector from the Camera's (updated) Eular Angles
  void updateCameraVectors();
};
