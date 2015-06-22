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
#include "defaults.h"
#include "camera.h"

/**
 * @brief Initialize program and run main loop
 */
class Game {
 public:
  Game();
  int run();

 private:
  GLenum fillmode_;
  bool keys_[1024];
  bool guiClosed_;
  bool leftMouseBtnPressed_;
  std::unique_ptr<TileManager> tileManager_;
  glm::vec3 currentPos_;

  Camera camera_;
  GLfloat deltaTime_;
  int frameCount_;
  GLfloat lastFrame_;
  GLfloat lastTime_;
  NoiseOptions options_;
  GLFWwindow *window_;

  // initialize OpenGL stuff
  int initializeGlfw();
  int initializeGlew();
  void initializeGl();

  void do_movement(const GLfloat &deltaTime);
  void getCurrentPosition();
  void setLeftMouseBtnPressed(bool isPressed);
  void showGui();
  void toggleGui();
  void toggleWireframe();

  // event callbacks
  static void key_callback(GLFWwindow *window, int key, int scancode,
                           int action, int mode);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void mouseBtn_callback(GLFWwindow *window, int button, int action,
                                int mod);
};
