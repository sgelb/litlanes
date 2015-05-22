#include "game.h"

Game::Game() : keys_{false}, fillmode_{GL_FILL} {
  camera_ = Camera(
      glm::vec3(Constants::TileWidth / 2, 60.0f, Constants::TileWidth / 2));
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

  initializeTiles();

  // Deltatime
  deltaTime_ = 0.0f; // Time between current frame and last frame
  lastFrame_ = 0.0f; // Time of last frame
  lastTime_ = 0.0f;  // Time since last fps-"second"
  frameCount_ = 0;

  // Game loop
  while (!glfwWindowShouldClose(window_)) {
    // Calculate deltatime of current frame
    GLfloat currentTime = glfwGetTime();
    deltaTime_ = currentTime - lastFrame_;
    lastFrame_ = currentTime;

    // Print fps on stdout
    /* printFps(); */

    // Check for events and trigger callbacks
    glfwPollEvents();
    do_movement(deltaTime_);

    // update camera position and create new tiles if neccessary
    getCurrentPosition();
    updateTiles();

    // Clear the colorbuffer
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Update  and render terrains
    for (size_t idx = 0; idx < terrains_.size(); idx++) {
      terrains_[idx]->update(deltaTime_);
      terrains_[idx]->render(camera_.getViewMatrix());
    }

    // Swap the screen buffers
    glfwSwapBuffers(window_);
  }

  // Clean up terrain
  for (size_t idx = 0; idx < terrains_.size(); idx++) {
    terrains_[idx]->cleanup();
  }

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window_);
  glfwTerminate();
  return 0;
}

void Game::printFps() {
  // framerate count
  frameCount_++;
  lastTime_ += deltaTime_;

  if (lastTime_ >= 1.0f) {
    std::cout << frameCount_ << " fps, " << (1000.0f / frameCount_)
              << "ms/frame" << std::endl;
    frameCount_ = 0;
    lastTime_ = 0.0f;
  }
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
  if (keys_[GLFW_KEY_X]) {
    fillmode_ = (fillmode_ == GL_FILL) ? GL_LINE : GL_FILL;
    glPolygonMode(GL_FRONT_AND_BACK, fillmode_);
  }
}

// Is called whenever a key is pressed/released via GLFW
void Game::key_callback(GLFWwindow *window, int key, int scancode, int action,
                        int mode) {

  Game *game = static_cast<Game *>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  }

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

void Game::initializeTiles() {

  // +----- x
  // |0 1 2
  // |3 4 5
  // |6 7 8
  // z

  for (size_t z = 0; z < 3; z++) {
    for (size_t x = 0; x < 3; x++) {
      std::shared_ptr<Terrain> terrain(new Terrain(x, z));
      terrain->setup();
      terrains_.push_back(std::move(terrain));
    }
  }
}

void Game::getCurrentPosition() {
  previousPos_ = currentPos_;
  currentPos_.x = camera_.getPosition().x;
  currentPos_.y = camera_.getPosition().z;
}

void Game::updateTiles() {
  int currentTileX = std::floor(currentPos_.x / Constants::TileWidth);
  int currentTileY = std::floor(currentPos_.y / Constants::TileWidth);
  int diffX = currentTileX - std::floor(previousPos_.x / Constants::TileWidth);
  int diffY = currentTileY - std::floor(previousPos_.y / Constants::TileWidth);

  // -------+-------+--------
  // (1,-1) | (1,0) |  (1,1)      x > 0
  // -------+-------+--------
  // (0,-1) | (0,0) |  (0,1)      x = 0
  // -------+-------+--------
  // (-1,-1)| (-1,0)| (-1,-1)     x < 0
  // -------+-------+--------
  //
  //  y < 0   y = 0    y > 0

  if (diffX == 0 and diffY == 0) {
    return; // we are still in middle tile, nothing to do
  }

  // std::shared_ptr<Terrain> terrain_:
  // +----- x
  // |0 1 2
  // |3 4 5
  // |6 7 8
  // z

  // Print current tile
  printCurrentTile();

  // Moving north
  if (diffX > 0) {
    // Move first two rows down
    // 0 1 2      6 7 8
    // 3 4 5  ->  0 1 2
    // 6 7 8      3 4 5
    std::rotate(terrains_.begin(), terrains_.begin() + 6, terrains_.end());

    // Update first row
    terrains_[0]->updateCoordinates(currentTileX + 1, currentTileY - 1);
    terrains_[1]->updateCoordinates(currentTileX + 1, currentTileY);
    terrains_[2]->updateCoordinates(currentTileX + 1, currentTileY + 1);
  }

  // Moving south
  if (diffX < 0) {
    // Move last two rows up
    // 0 1 2      3 4 5
    // 3 4 5  ->  6 7 8
    // 6 7 8      0 1 2
    std::rotate(terrains_.begin(), terrains_.begin() + 3, terrains_.end());

    // Update last row
    terrains_[6]->updateCoordinates(currentTileX - 1, currentTileY - 1);
    terrains_[7]->updateCoordinates(currentTileX - 1, currentTileY);
    terrains_[8]->updateCoordinates(currentTileX - 1, currentTileY + 1);
  }

  // Moving west
  if (diffY < 0) {
    // Move first two columns right
    // 0 1 2      2 0 1
    // 3 4 5  ->  5 3 4
    // 6 7 8      8 6 7
    std::rotate(terrains_.begin(), terrains_.begin() + 2,
                terrains_.begin() + 3);
    std::rotate(terrains_.begin() + 3, terrains_.begin() + 5,
                terrains_.begin() + 6);
    std::rotate(terrains_.begin() + 6, terrains_.begin() + 8, terrains_.end());

    // Update first column
    terrains_[0]->updateCoordinates(currentTileX + 1, currentTileY - 1);
    terrains_[3]->updateCoordinates(currentTileX, currentTileY - 1);
    terrains_[6]->updateCoordinates(currentTileX - 1, currentTileY - 1);
  }

  // Moving east
  if (diffY > 0) {
    // Move last two columns left
    // 0 1 2      1 2 0
    // 3 4 5  ->  4 5 3
    // 6 7 8      7 8 6
    std::rotate(terrains_.begin(), terrains_.begin() + 1,
                terrains_.begin() + 3);
    std::rotate(terrains_.begin() + 3, terrains_.begin() + 4,
                terrains_.begin() + 6);
    std::rotate(terrains_.begin() + 6, terrains_.begin() + 7, terrains_.end());

    // Update last column
    terrains_[2]->updateCoordinates(currentTileX + 1, currentTileY + 1);
    terrains_[5]->updateCoordinates(currentTileX, currentTileY + 1);
    terrains_[8]->updateCoordinates(currentTileX - 1, currentTileY + 1);
  }
}

void Game::printCurrentTile() {
  std::cout << std::floor(currentPos_.x / Constants::TileWidth) << ", "
            << std::floor(currentPos_.y / Constants::TileWidth) << std::endl;
}

