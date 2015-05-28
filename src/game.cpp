#include "game.h"

Game::Game() : keys_{false}, fillmode_{GL_FILL} {
  // position camera in center of 3*3 tiles
  GLfloat coord = 3 * Constants::TileWidth / 2;
  currentPos_ = glm::vec3(coord, 0.0f, coord);
  camera_ = Camera(glm::vec3(currentPos_.x, 60.0f, currentPos_.z));
  tileManager_ = TileManager();
  cameraFreeze_ = false;
}

int Game::run() {
  // Initalize
  if (!initializeGlfw()) {
    return 1;
  }
  if (GLEW_OK != initializeGlew()) {
    return 2;
  }
  initializeGl();
  tileManager_.initialize(currentPos_);

  ImGui_ImplGlfwGL3_Init(window_, false);

  deltaTime_ = 0.0f; // Time between current and last frame
  lastFrame_ = 0.0f; // Time at last frame
  lastTime_ = 0.0f;  // Time since last fps-"second"
  frameCount_ = 0;

  // is GUI open?
  guiOpen_ = true;
  int e = 0;
  int octaves = 1;

  // Game loop
  while (!glfwWindowShouldClose(window_)) {
    // Calculate deltatime of current frame
    GLfloat currentTime = glfwGetTime();
    deltaTime_ = currentTime - lastFrame_;
    lastFrame_ = currentTime;

    // Check for events
    glfwPollEvents();

    // GUI
    // TODO: refactor/move
    if (guiOpen_) {
      ImGui_ImplGlfwGL3_NewFrame();
      ImGui::BeginPopup(&guiOpen_);

      // Style
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

      // Position
      ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_FirstUseEver);

      // FPS
      ImGui::Text("Avg %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

      // Current tile
      int xTile = std::floor(currentPos_.x / Constants::TileWidth);
      int zTile = std::floor(currentPos_.z / Constants::TileWidth);
      ImGui::Text("Current tile: %i,%i", xTile, zTile);

      // Keys
      if (ImGui::CollapsingHeader("Keys")) {
        ImGui::BulletText("TAB toggles this window");
        ImGui::BulletText("F freezes camera movement");
        ImGui::BulletText("Move around with A, S, D, W, E and Q");
        ImGui::BulletText("Use mouse to look around");
      }

      // Algorithm
      if (ImGui::CollapsingHeader("Algorithms")) {
        if (ImGui::RadioButton("Perlin Noise", &e, Constants::Perlin)) {
          tileManager_.changeTileAlgorithm(Constants::Perlin);
        };
        if (ImGui::RadioButton("Ridged-Multifractal Noise", &e,
                               Constants::RidgedMulti)) {
          tileManager_.changeTileAlgorithm(Constants::RidgedMulti);
        };
        if (ImGui::RadioButton("Billow", &e, Constants::Billow)) {
          tileManager_.changeTileAlgorithm(Constants::Billow);
        };
        if (ImGui::RadioButton("Diamond-Square", &e,
                               Constants::DiamondSquare)) {
          tileManager_.changeTileAlgorithm(Constants::DiamondSquare);
        };
        if (ImGui::RadioButton("Random", &e, Constants::Random)) {
          tileManager_.changeTileAlgorithm(Constants::Random);
        };
      }

      // Algorithm Options
      // TODO: show algorithm-specific options
      if (ImGui::CollapsingHeader("Algorithm Options")) {
        if (ImGui::SliderInt("Octaves", &octaves, 1, 6)) {
          tileManager_.setOctaveCount(octaves);
        }
      }
 

      // TODO:
      // - Simplex Algo
      // - wireframe
      // - coloring options
      // - GoTo tile

      ImGui::EndPopup();
    }

    // Move
    do_movement(deltaTime_);

    // Get current camera position
    getCurrentPosition();

    // Clear color- and depth buffer
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render tiles
    tileManager_.update(currentPos_);
    tileManager_.renderAll(deltaTime_, camera_.getViewMatrix());

    // Render GUI
    if (guiOpen_) {
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
  tileManager_.cleanUp();

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

// Is called whenever a key is pressed/released via GLFW
void Game::key_callback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  // set commands
  if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
    game->toggleGui();
  }
  if (key == GLFW_KEY_X && action == GLFW_PRESS) {
    game->toggleWireframe();
  }
  if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    game->toggleCameraFreeze();
  }

  // set movement keys.
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      game->keys_[key] = true;
    } else if (action == GLFW_RELEASE) {
      game->keys_[key] = false;
    }
  }
}

void Game::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static bool firstMouse = true;
  static GLfloat lastX;
  static GLfloat lastY;

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (ImGui::GetIO().WantCaptureMouse || game->cameraFreeze_) {
    return;
  }


  if (firstMouse) {
    lastX = static_cast<GLfloat>(xpos);
    lastY = static_cast<GLfloat>(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = xpos - lastX;
  // Reversed since y-coordinates go from bottom to left
  GLfloat yoffset = lastY - ypos;
  lastX = static_cast<GLfloat>(xpos);
  lastY = static_cast<GLfloat>(ypos);

  game->camera_.processMouseMovement(xoffset, yoffset);
}

int Game::initializeGlfw() {
  // Init GLFW
  if (!glfwInit()) {
    return GL_FALSE;
  }
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  window_ = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight,
                             "litlanesfoss", nullptr, nullptr);
  glfwMakeContextCurrent(window_);

  // Set the required callback functions
  glfwSetWindowUserPointer(window_, this);
  glfwSetKeyCallback(window_, Game::key_callback);
  glfwSetCursorPosCallback(window_, Game::mouse_callback);
  glfwSetMouseButtonCallback(window_, ImGui_ImplGlfwGL3_MouseButtonCallback);

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
  glViewport(0, 0, Constants::WindowWidth, Constants::WindowHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
}

void Game::getCurrentPosition() {
  /* previousPos_ = currentPos_; */
  currentPos_.x = camera_.getPosition().x;
  currentPos_.z = camera_.getPosition().z;
}

void Game::toggleGui() {
  guiOpen_ = !guiOpen_;
}

void Game::toggleCameraFreeze() {
  cameraFreeze_ = !cameraFreeze_;
}

void Game::toggleWireframe() {
  // TODO: show wireframe in solid color?
  fillmode_ = (fillmode_ == GL_FILL) ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, fillmode_);
}
