#include <camera.h>

// Constructor with vectors
Camera::Camera(glm::vec3 position, glm::vec3 up, GLfloat yaw, GLfloat pitch) :
  front(glm::vec3(0.0f, 0.0f, -1.0f)),
  movementSpeed(SPEED),
  mouseSensitivity(SENSITIVTY),
  zoom(ZOOM) {
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;
    this->updateCameraVectors();
}

// Delegate constructor with scalar values
Camera::Camera(GLfloat posX, GLfloat posY, GLfloat posZ, GLfloat upX,
    GLfloat upY, GLfloat upZ, GLfloat yaw, GLfloat pitch) :
  Camera(glm::vec3(posX, posY, posZ), glm::vec3(upX, upY, upZ), yaw, pitch) {}


// Returns the view matrix calculated using Eular Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix() {
  return glm::lookAt(this->position, this->position + this->front, this->up);
}

// Processes input received from any keyboard-like input system. Accepts input
// parameter in the form of camera defined ENUM (to abstract it from windowing
// systems)
void Camera::ProcessKeyboard(CameraMovement direction, GLfloat deltaTime) {
  GLfloat velocity = this->movementSpeed * deltaTime;
  if (direction == CameraMovement::Forward)
    this->position += this->front * velocity;
  if (direction == CameraMovement::Backward)
    this->position -= this->front * velocity;
  if (direction == CameraMovement::Left)
    this->position -= this->right * velocity;
  if (direction == CameraMovement::Right)
    this->position += this->right * velocity;
}

// Processes input received from a mouse input system. Expects the offset
// value in both the x and y direction.
void Camera::ProcessMouseMovement(GLfloat xoffset, GLfloat yoffset,
    GLboolean constrainPitch) {
  xoffset *= this->mouseSensitivity;
  yoffset *= this->mouseSensitivity;

  this->yaw += xoffset;
  this->pitch += yoffset;

  // Make sure that when pitch is out of bounds, screen doesn't get flipped
  if (constrainPitch) {
    if (this->pitch > 89.0f)
      this->pitch = 89.0f;
    if (this->pitch < -89.0f)
      this->pitch = -89.0f;
  }

  // Update Front, Right and Up Vectors using the updated Eular angles
  this->updateCameraVectors();
}

// Processes input received from a mouse scroll-wheel event. Only requires
// input on the vertical wheel-axis
void Camera::ProcessMouseScroll(GLfloat yoffset) {
  if (this->zoom >= 1.0f && this->zoom <= 45.0f)
    this->zoom -= yoffset;
  if (this->zoom <= 1.0f)
    this->zoom = 1.0f;
  if (this->zoom >= 45.0f)
    this->zoom = 45.0f;
}

// Calculates the front vector from the Camera's (updated) Eular Angles
void Camera::updateCameraVectors() {
  // Calculate the new Front vector
  glm::vec3 front;
  front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  front.y = sin(glm::radians(this->pitch));
  front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
  this->front = glm::normalize(front);
  // Also re-calculate the Right and Up vector
  this->right = glm::normalize(
      glm::cross(this->front, this->worldUp));
  // Normalize the vectors, because their length gets closer to 0 the more you
  // look up or down which results in slower movement.
  this->up = glm::normalize(glm::cross(this->right, this->front));
}

