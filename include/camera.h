#pragma once

#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "defaults.h"

class Camera {
 public:
  enum CameraMovement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

  Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
         glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
         GLfloat yaw = Defaults::Yaw, GLfloat pitch = Defaults::Pitch);

  glm::mat4 getViewMatrix();
  void processKeyboard(CameraMovement direction, GLfloat deltaTime);
  void processMouseMovement(GLfloat xoffset, GLfloat yoffset);
  glm::vec3 getPosition();
  void setMovementSpeed(const GLfloat &speed);

 private:
  glm::vec3 position_;
  glm::vec3 front_;
  glm::vec3 up_;
  glm::vec3 right_;
  glm::vec3 worldUp_;

  GLfloat yaw_;
  GLfloat pitch_;

  GLfloat movementSpeed_;
  GLfloat mouseSensitivity_;

  void updateCameraVectors();
};
