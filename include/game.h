#pragma once

#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw_gl3.h>

#include "tileManager.h"
#include "constants.h"
#include "camera.h"

/**
 * @brief Initialize program and run main loop
 */
class Game {
 public:
  /**
   * @brief Constructor
   */
  Game();
  /**
   * @brief Main loop
   *
   * @return Exit value
   */
  int run();

 private:
  bool keys_[1024] = {false};

  Camera camera_;
  GLFWwindow *window_;
  TileManager tileManager_;
  bool guiClosed_;
  bool leftMouseBtnPressed_;
  GLfloat deltaTime_;
  GLfloat lastFrame_;
  GLfloat lastTime_;
  int frameCount_;
  GLenum fillmode_;
  glm::vec3 currentPos_;
  NoiseOptions options_;

  int initializeGlfw();
  int initializeGlew();
  void initializeGl();
  void toggleGui();
  void toggleWireframe();
  void getCurrentPosition();
  void do_movement(const GLfloat &deltaTime);
  void showGui();
  void setLeftMouseBtnPressed(bool isPressed);
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mode);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void mouseBtn_callback(GLFWwindow *window, int button, int action, int mod);
};
