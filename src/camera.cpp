#include "camera.h"

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
    : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed_(Constants::Speed),
      mouseSensitivity_(Constants::Sensitivty),
      zoom_(Constants::Zoom) {
  this->position_ = position;
  this->worldUp_ = up;
  this->yaw_ = yaw;
  this->pitch_ = pitch;
  this->updateCameraVectors();
}

// Delegate constructor with scalar values
Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,
               GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch)
    : Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw,
             pitch) {
}

// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(this->position_, this->position_ + this->front_, this->up_);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::processKeyboard(CameraMovement direction, GLfloat deltaTime) {
  GLfloat velocity_ = this->movementSpeed_ * deltaTime;

  switch (direction) {
    case CameraMovement::FORWARD:
      this->position_ += this->front_ * velocity_;
      break;
    case CameraMovement::BACKWARD:
      this->position_ -= this->front_ * velocity_;
      break;
    case CameraMovement::LEFT:
      this->position_ -= this->right_ * velocity_;
      break;
    case CameraMovement::RIGHT:
      this->position_ += this->right_ * velocity_;
      break;
    case CameraMovement::UP:
      this->position_ += this->worldUp_ * velocity_;
      break;
    case CameraMovement::DOWN:
      this->position_ -= this->worldUp_ * velocity_;
      break;
    default:
      break;
  }
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset) {
  xoffset *= this->mouseSensitivity_;
  yoffset *= this->mouseSensitivity_;

  this->yaw_ += xoffset;  // rotate around local y-axis
  this->pitch_ += yoffset;  // rotate around local x-axis

  // prevent camera to flip over
  if (this->pitch_ > 89.0f) {
    this->pitch_ = 89.0f;
  }
  if (this->pitch_ < -89.0f) {
    this->pitch_ = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::processMouseScroll(GLfloat yoffset) {
  // FIXME
  if (this->zoom_ >= 1.0f && this->zoom_ <= 45.0f)
    this->zoom_ -= yoffset;
  if (this->zoom_ <= 1.0f)
    this->zoom_ = 1.0f;
  if (this->zoom_ >= 45.0f)
    this->zoom_ = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
  front.y = sin(glm::radians(this->pitch_));
  front.z = sin(glm::radians(this->yaw_)) * cos(glm::radians(this->pitch_));
  this->front_ = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  this->right_ = glm::normalize(glm::cross(this->front_, this->worldUp_));
  // Normalize the vectors, because their length gets closer to 0 the more you
  // look up or down which results in slower movement.
  this->up_ = glm::normalize(glm::cross(this->right_, this->front_));
}

GLfloat Camera::getZoom() {
  return this->zoom_;
}
