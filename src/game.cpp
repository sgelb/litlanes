#include "game.h"

Game::Game() : keys_{false}, fillmode_{GL_FILL} {
  camera_ = Camera(
      glm::vec3(Constants::TileWidth / 2, 30.0f, Constants::TileWidth / 2));
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

  Terrain terrain;

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
    printFps();

    // Check for events and trigger callbacks
    glfwPollEvents();
    do_movement(deltaTime_);

    // Update  and render terrain
    terrain.update(deltaTime_);
    terrain.render(camera_.getViewMatrix());

    // Swap the screen buffers
    glfwSwapBuffers(window_);
  }

  // Clean up terrain
  terrain.cleanup();

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
