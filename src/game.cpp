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

#include "game.h"

Game::Game()
    : fillmode_(GL_FILL),
      keys_{0},
      guiClosed_(false),
      leftMouseBtnPressed_(false),
      tileManager_(std::unique_ptr<TileManager>(new TileManager)),
      currentPos_(Defaults::CameraPosition) {
  camera_ = Camera(
      glm::vec3(currentPos_.x, 3.5 * Defaults::TileWidth, currentPos_.z));
}

int Game::run() {
  // Initialize
  if (!initializeGlfw()) {
    return 1;
  }
  if (GLEW_OK != initializeGlew()) {
    return 2;
  }
  initializeGl();
  tileManager_->initialize(currentPos_);
  options_ = tileManager_->getOptions();
  ImGui_ImplGlfwGL3_Init(window_, true);

  deltaTime_ = 0.0f; // Time between current and last frame
  lastFrame_ = 0.0f; // Time at last frame
  lastTime_ = 0.0f;  // Time since last fps-"second"
  frameCount_ = 0;

  // Game loop
  while (!glfwWindowShouldClose(window_)) {
    // Calculate deltatime of current frame
    GLfloat currentTime = glfwGetTime();
    deltaTime_ = currentTime - lastFrame_;
    lastFrame_ = currentTime;

    // Check for events
    glfwPollEvents();

    // GUI
    showGui();

    // Move
    do_movement(deltaTime_);

    // Get current camera position
    getCurrentPosition();

    // Clear color- and depth buffer
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render tiles
    tileManager_->update(currentPos_);
    tileManager_->renderAll(deltaTime_, camera_.getViewMatrix());

    // Render GUI
    if (!guiClosed_) {
      // ignore wireframe setting for gui
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
      ImGui::Render();
      glPolygonMode(GL_FRONT_AND_BACK, fillmode_);
    }

    // Swap the screen buffers
    glfwSwapBuffers(window_);
  }

  // Clean up imgui
  ImGui_ImplGlfwGL3_Shutdown();

  // Clean up tiles
  tileManager_->cleanUp();

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window_);
  glfwTerminate();
  return 0;
}

void Game::do_movement(const GLfloat &deltaTime) {
  // Camera keyboard controls
  if (keys_[GLFW_KEY_W]) {
    camera_.processKeyboard(Camera::FORWARD, deltaTime);
  }
  if (keys_[GLFW_KEY_S]) {
    camera_.processKeyboard(Camera::BACKWARD, deltaTime);
  }
  if (keys_[GLFW_KEY_A]) {
    camera_.processKeyboard(Camera::LEFT, deltaTime);
  }
  if (keys_[GLFW_KEY_D]) {
    camera_.processKeyboard(Camera::RIGHT, deltaTime);
  }
  if (keys_[GLFW_KEY_E]) {
    camera_.processKeyboard(Camera::UP, deltaTime);
  }
  if (keys_[GLFW_KEY_Q]) {
    camera_.processKeyboard(Camera::DOWN, deltaTime);
  }
}

void Game::key_callback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {

  // Quit program
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (ImGui::GetIO().WantCaptureKeyboard) {
    return;
  }

  // Toggle GUI
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    game->toggleGui();
  }

  // Toggle wireframe
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    game->toggleWireframe();
  }

  // Set movement keys
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      game->keys_[key] = true;
    } else if (action == GLFW_RELEASE) {
      game->keys_[key] = false;
    }
  }
}

void Game::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static bool resetMouse = true;
  static GLfloat lastX;
  static GLfloat lastY;

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (ImGui::GetIO().WantCaptureMouse || !game->leftMouseBtnPressed_) {
    resetMouse = true;
    return;
  }

  if (resetMouse) {
    lastX = static_cast<GLfloat>(xpos);
    lastY = static_cast<GLfloat>(ypos);
    resetMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  // Reversed since y-coordinates go from bottom to left
  GLfloat yoffset = lastY - ypos;
  lastX = static_cast<GLfloat>(xpos);
  lastY = static_cast<GLfloat>(ypos);

  game->camera_.processMouseMovement(xoffset, yoffset);
}

void Game::mouseBtn_callback(GLFWwindow *window, int button, int action,
                             int mods) {

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    game->setLeftMouseBtnPressed(true);
    return;
  }

  if (action == GLFW_RELEASE) {
    GLFWcursor *handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    game->setLeftMouseBtnPressed(false);
    return;
  }
}

void Game::setLeftMouseBtnPressed(bool isPressed) {
  leftMouseBtnPressed_ = isPressed;
}

int Game::initializeGlfw() {
  // Init GLFW
  if (!glfwInit()) {
    std::cerr << "Could not initialize GLFW" << std::endl;
    return GL_FALSE;
  }
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  window_ = glfwCreateWindow(Defaults::WindowWidth, Defaults::WindowHeight,
                             "litlanesfoss", nullptr, nullptr);
  if (!window_) {
    std::cerr << "Could not create GLFWwindow. Requires OpenGL 3.3 or higher."
              << std::endl;
    return GL_FALSE;
  }
  glfwMakeContextCurrent(window_);

  // Set the required callback functions
  glfwSetWindowUserPointer(window_, this);
  glfwSetKeyCallback(window_, Game::key_callback);
  glfwSetCursorPosCallback(window_, Game::mouse_callback);
  glfwSetMouseButtonCallback(window_, Game::mouseBtn_callback);

  // GLFW Options
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  return GL_TRUE;
}

int Game::initializeGlew() {
  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  return glewInit();
}

void Game::initializeGl() {
  // Define the viewport dimensions
  glViewport(0, 0, Defaults::WindowWidth, Defaults::WindowHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Game::getCurrentPosition() {
  /* previousPos_ = currentPos_; */
  currentPos_.x = camera_.getPosition().x;
  currentPos_.z = camera_.getPosition().z;
}

void Game::toggleGui() {
  guiClosed_ = !guiClosed_;
}

void Game::toggleWireframe() {
  // TODO: change shader to show wireframe in solid color?
  fillmode_ = (fillmode_ == GL_FILL) ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, fillmode_);
}

void Game::showGui() {
  static int algorithm = Defaults::Perlin;

  if (guiClosed_) {
    return;
  }

  ImGui_ImplGlfwGL3_NewFrame();
  ImGui::BeginPopup(&guiClosed_);

  // Style. No rounded corners
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

  // Position. TopLeft
  ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);

  // Show FPS
  ImGui::Text("Avg %.3f ms/frame (%.1f FPS)",
              1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

  // Show current tile
  int xTile = std::floor(currentPos_.x / Defaults::TileWidth);
  int zTile = std::floor(currentPos_.z / Defaults::TileWidth);
  ImGui::Text("Current tile: %i,%i", xTile, zTile);

  // Keys
  if (ImGui::CollapsingHeader("Keys")) {
    ImGui::BulletText("Hold left mouse button to look around");
    ImGui::BulletText("Move with <A>, <S>, <D>, <W>, <E> and <Q>");
    ImGui::BulletText("Toggle wireframe with <X>");
    ImGui::BulletText("Toggle this menu with <TAB>");
  }

  if (ImGui::CollapsingHeader("Map Options")) {
    bool showSea = tileManager_->getShowSea();
    float seaLevel = tileManager_->getSeaLevel();

    if (ImGui::Checkbox("Show sea", &showSea)) {
      tileManager_->setShowSea(showSea);
    }

    if (showSea) {
      if (ImGui::SliderFloat("Sea level", &seaLevel, 0,
                             Defaults::MaxMeshHeight)) {
        tileManager_->setSeaLevel(seaLevel);
      }
    }
  }

  // Algorithm
  if (ImGui::CollapsingHeader("Algorithms")) {

    // This code to test if an option is set is not very readable, but i like
    // it. | is the bitwise OR-operator. It returns true if at least one of the
    // two compared bits is true. ImGui::RadioButton returns true if pressed.
    // If this happens just once, btnPressed stays true, no matter the
    // following results.

    bool btnPressed = false;
    btnPressed |= (ImGui::RadioButton("Perlin Noise / fBm", &algorithm,
                                      Defaults::Perlin));
    btnPressed |= (ImGui::RadioButton("Ridged-Multifractal Noise", &algorithm,
                                      Defaults::RidgedMulti));
    btnPressed |=
        (ImGui::RadioButton("Billow Noise", &algorithm, Defaults::Billow));
    btnPressed |=
        (ImGui::RadioButton("Random Noise", &algorithm, Defaults::Random));

    if (btnPressed) {
      tileManager_->setTileAlgorithm(algorithm);
      options_ = tileManager_->getOptions();
    }

    // Algorithm Options
    bool optsChanged = false;

    // TODO: better handling of available options for different algorithms
    if (algorithm != Defaults::Random) {
      optsChanged |= (ImGui::InputInt("Seed", &options_.seed, 1));
      optsChanged |=
          (ImGui::SliderFloat("Frequency", &options_.frequency, 1, 6));
      optsChanged |=
          (ImGui::SliderFloat("Lacunarity", &options_.lacunarity, 0, 4));
      optsChanged |= (ImGui::SliderInt("Octaves", &options_.octaveCount, 1, 6));

      if (algorithm != Defaults::RidgedMulti) {
        optsChanged |=
            (ImGui::SliderFloat("Persistence", &options_.persistence, 0, 1));
      }
    }

    if (optsChanged) {
      tileManager_->setTileAlgorithmOptions(options_);
    }
  }

  ImGui::EndPopup();
}
