#pragma once

#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "defaults.h"

/**
 * @brief Process keyboard/mouse input and calculate view matrix for OpenGL
 */
class Camera {
 public:
  /**
   * @brief Define possible directions for camera movement
   */
  enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

  /**
   * @brief Constructor using vectors
   *
   * @param position camera position
   * @param vector pointing up in world space
   * @param yaw Euler angle around y-axis
   * @param pitch Euler angle around x-axis
   */
  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = Defaults::Yaw, GLfloat pitch = Defaults::Pitch);

  /**
   * @brief Return calculated view matrix
   *
   * @return view matrix
   */
  glm::mat4 getViewMatrix();

  /**
   * @brief Process keyboard input for movement and calculate new position
   *
   * @param direction direction of movement
   * @param deltaTime duration of movement
   */
  void processKeyboard(CameraMovement direction, GLfloat deltaTime);

  /**
   * @brief Process mouse input for viewing direction and calculate new yaw and
   * pitch values
   *
   * @param xoffset x-offset of mouse
   * @param yoffset y-offset of mouse
   */
  void processMouseMovement(GLfloat xoffset, GLfloat yoffset);

  glm::vec3 getPosition();

  void setMovementSpeed(const GLfloat &speed);

 private:
  // Camera Attributes
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 worldUp_;

  // Eular Angles
  GLfloat yaw_;
  GLfloat pitch_;

  // Camera options
  GLfloat movementSpeed_;
  GLfloat mouseSensitivity_;

  // Calculates the front vector from the Camera's (updated) Eular Angles
  void updateCameraVectors();
};
