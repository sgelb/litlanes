#include "game.h"

Game::Game() : fillmode_(GL_FILL) {
  // position camera in center of 3*3 tiles
  GLfloat coord = 3 * Constants::TileWidth / 2;
  currentPos_ = glm::vec3(coord, 0.0f, coord);
  camera_ = Camera(glm::vec3(currentPos_.x, 60.0f, currentPos_.z));
  tileManager_ = TileManager();
  cameraFreeze_ = false;
  guiClosed_ = false;
  leftMouseBtnPressed_ = false;
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
  options_ = tileManager_.getOptions();

  ImGui_ImplGlfwGL3_Init(window_, false);

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
    glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update and render tiles
    tileManager_.update(currentPos_);
    tileManager_.renderAll(deltaTime_, camera_.getViewMatrix());

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

  if (ImGui::GetIO().WantCaptureMouse) {
    return;
  }

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
    game->setLeftMouseBtnPressed(true);
    return;
  }

  if (action == GLFW_RELEASE) {
    GLFWcursor* handCursor = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
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
  window_ = glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight,
                             "litlanesfoss", nullptr, nullptr);
  if (!window_) {
    std::cerr << "Could not create GLFWwindow. Requires OpenGL 3.3 or higher." << std::endl;
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
  glViewport(0, 0, Constants::WindowWidth, Constants::WindowHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnable(GL_PROGRAM_POINT_SIZE);
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(30.0f);
  glPointSize(20.0f);
}

void Game::getCurrentPosition() {
  /* previousPos_ = currentPos_; */
  currentPos_.x = camera_.getPosition().x;
  currentPos_.z = camera_.getPosition().z;
}

void Game::toggleGui() {
  guiClosed_ = !guiClosed_;
}

void Game::toggleCameraFreeze() {
  cameraFreeze_ = !cameraFreeze_;
}

void Game::toggleWireframe() {
  // TODO: change shader to show wireframe in solid color?
  fillmode_ = (fillmode_ == GL_FILL) ? GL_LINE : GL_FILL;
  glPolygonMode(GL_FRONT_AND_BACK, fillmode_);
}

void Game::showGui() {
  static int algorithm = Constants::Perlin;

  if (guiClosed_) {
    return;
  }

  ImGui_ImplGlfwGL3_NewFrame();
  ImGui::BeginPopup(&guiClosed_);

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
    ImGui::BulletText("<TAB> toggles GUI");
    ImGui::BulletText("Move around with <A>, <S>, <D>, <W>, <E> and <Q>");
    ImGui::BulletText("Look around with mouse");
    ImGui::BulletText("<F> freezes mouse");
  }

  // Camera options
  if (ImGui::CollapsingHeader("Camera")) {
    /* (ImGui::SliderFloat("Speed", &camera_->speed, 1, 100)); */
  }

  if (ImGui::CollapsingHeader("Map Options")) {
    float seaLevel = tileManager_.getSeaLevel();
    if (ImGui::SliderFloat("Sea level", &seaLevel, 0, Constants::MaxMeshHeight)) {
      tileManager_.setSeaLevel(seaLevel);
    }
  }

  // Algorithm
  if (ImGui::CollapsingHeader("Algorithms")) {

    // This code is not very readable, but i like it. | is the bitwise
    // OR-operator. It returns true if at least one of the two compared bits is
    // true. ImGui::RadioButton returns true if pressed. If this happens just
    // once, btnPressed stays true, no matter the following results.

    bool btnPressed = false;
    btnPressed |=
      (ImGui::RadioButton("Perlin Noise", &algorithm, Constants::Perlin));
    btnPressed |= (ImGui::RadioButton("Ridged-Multifractal Noise", &algorithm,
          Constants::RidgedMulti));
    btnPressed |= (ImGui::RadioButton("Billow", &algorithm, Constants::Billow));
    btnPressed |= (ImGui::RadioButton("Random", &algorithm, Constants::Random));

    if (btnPressed) {
      tileManager_.setTileAlgorithm(algorithm);
      options_ = tileManager_.getOptions();
    }

    // Algorithm Options
    bool optsChanged = false;

    // TODO: better handling of available options
    if (algorithm != Constants::Random) {

      optsChanged |= (ImGui::SliderFloat("Frequency", &options_.frequency, 1, 6));
      optsChanged |=
        (ImGui::SliderFloat("Lacunarity", &options_.lacunarity, 0, 4));
      optsChanged |= (ImGui::SliderInt("Octaves", &options_.octaveCount, 1, 6));
      optsChanged |= (ImGui::SliderInt("Seed", &options_.seed, 1, 6));

      if (algorithm != Constants::RidgedMulti) {
        optsChanged |=
          (ImGui::SliderFloat("Persistence", &options_.persistence, 0, 1));
      }
    }

    if (optsChanged) {
      tileManager_.setTileAlgorithmOptions(options_);
    }
  }

  ImGui::EndPopup();
}
