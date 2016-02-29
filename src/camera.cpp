/*
 * Copyright (C) 2016 sgelb
 *
 * This file is part of litlanes.
 *
 * litlanes is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * litlanes is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with litlanes.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch)
    : front_(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed_(Defaults::Speed),
      mouseSensitivity_(Defaults::Sensitivity) {
  position_ = position;
  worldUp_ = up;
  yaw_ = yaw;
  pitch_ = pitch;
  updateCameraVectors();
}

// calculate and return the view matrix
glm::mat4 Camera::getViewMatrix() {
  return glm::lookAt(position_, position_ + front_, up_);
}

// processes keyboard movement, moves camera position
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

// process mouse input and update camera angles
void Camera::processMouseMovement(GLfloat xoffset, GLfloat yoffset) {
  xoffset *= mouseSensitivity_;
  yoffset *= mouseSensitivity_;

  yaw_ += xoffset;   // rotate around local y-axis
  pitch_ += yoffset; // rotate around local x-axis

  // prevent camera from flipping over when looking down
  if (pitch_ > 89.0f) {
    pitch_ = 89.0f;
  }

  if (pitch_ < -89.0f) {
    pitch_ = -89.0f;
  }

  updateCameraVectors();
}

glm::vec3 Camera::getPosition() {
  return position_;
}

void Camera::setMovementSpeed(const GLfloat &speed) {
  movementSpeed_ = speed;
}

void Camera::updateCameraVectors() {
  // calculate the new front_ vector
  glm::vec3 front;
  front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front.y = sin(glm::radians(pitch_));
  front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
  front_ = glm::normalize(front);

  // calculate new values for right_ and up_
  right_ = glm::normalize(glm::cross(front_, worldUp_));
  up_ = glm::normalize(glm::cross(right_, front_));
}
