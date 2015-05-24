#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
    : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed_(Constants::Speed),
      mouseSensitivity_(Constants::Sensitivty) {
  position_ = position;
  worldUp_ = up;
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::processKeyboard(CameraMovement direction, GLfloat deltaTime) {
  GLfloat velocity_ = movementSpeed_ * deltaTime;

  switch (direction) {
  case CameraMovement::FORWARD:
    position_ += front_ * velocity_;
    break;
  case CameraMovement::BACKWARD:
    position_ -= front_ * velocity_;
    break;
  case CameraMovement::LEFT:
    position_ -= right_ * velocity_;
    break;
  case CameraMovement::RIGHT:
    position_ += right_ * velocity_;
    break;
  case CameraMovement::UP:
    position_ += worldUp_ * velocity_;
    break;
  case CameraMovement::DOWN:
    position_ -= worldUp_ * velocity_;
    break;
  default:
    break;
  }
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset) {
  xoffset *= mouseSensitivity_;
  yoffset *= mouseSensitivity_;

  yaw_ += xoffset;   // rotate around local y-axis
  pitch_ += yoffset; // rotate around local x-axis

  // prevent camera to flip over
  if (pitch_ > 89.0f) {
    pitch_ = 89.0f;
  }
  if (pitch_ < -89.0f) {
    pitch_ = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  updateCameraVectors();
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front_ = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  right_ = glm::normalize(glm::cross(front_, worldUp_));
  // Normalize the vectors, because their length gets closer to 0 the more you
  // look up or down which results in slower movement.
  up_ = glm::normalize(glm::cross(right_, front_));
}

// Get current camera position
glm::vec3 Camera::getPosition() {
  return position_;
}
