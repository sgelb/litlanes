#include "game.h"


Game::Game() : keys_{false} {
  camera_ = Camera(glm::vec3(Constants::TileWidth / 2, 60.0f,
                        Constants::TileWidth / 2));
}

int Game::run() {

  // Initalize
  initializeGlfw();
  initializeGlew();
  initializeGl();

  // CREATE STUFF
  ///////////////

  // Set up vertex data (and buffer(s)) and attribute pointers
  Terrain terrain;
  terrain.create();
  auto vertices = terrain.getVertices();

  Quadtree quadtree;
  auto indices = quadtree.getIndicesOfLevel(Constants::MaximumLod);

// ------>
  // Build and compile our shader program
  Shader terraShader("shader/default.vert", "shader/default.frag");

  GLuint VAO;
  glGenVertexArrays(1, &VAO);
  GLuint VBO; // Vertex Buffer Object
  glGenBuffers(1, &VBO);
  GLuint EBO; // Element Buffer Object
  glGenBuffers(1, &EBO);

  // Bind VAO first,
  glBindVertexArray(VAO);

  // then bind and set vertex buffers
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat),
               &vertices.front(), GL_STATIC_DRAW);

  // the element buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
               &indices.front(), GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        reinterpret_cast<GLvoid *>(0));
  glEnableVertexAttribArray(0);
  // Color attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                        reinterpret_cast<GLvoid *>(3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(1);

  // Unbind buffers/arrays to prevent strange bugs
  glBindVertexArray(0);
// <------


  // Deltatime
  deltaTime_ = 0.0f; // Time between current frame and last frame
  lastFrame_ = 0.0f; // Time of last frame
  lastTime_ = 0.0f;  // Time since last fps-"second"
  frameCount_ = 0;

  // Game loop
  while (!glfwWindowShouldClose(window_)) {
    // TODO: refactor in processInput(), update(), render()

    // Calculate deltatime of current frame
    GLfloat currentTime = glfwGetTime();
    deltaTime_ = currentTime - lastFrame_;
    lastFrame_ = currentTime;

    // Print fps on stdout
    printFps();

    // Check for events and trigger callbacks
    glfwPollEvents();
    do_movement(deltaTime_);

    // TODO: move to terrain-update/render-method
    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate shader
    terraShader.use();

    // Camera/View transformation
    glm::mat4 view;
    view = camera_.getViewMatrix();

    // Projection
    glm::mat4 projection;
    projection = glm::perspective(
        camera_.getZoom(), static_cast<GLfloat>(Constants::WindowWidth) /
                              static_cast<GLfloat>(Constants::WindowHeight),
        Constants::NearPlane, Constants::FarPlane);

    // Get the uniform locations
    GLint modelLoc = glGetUniformLocation(terraShader.getProgram(), "model");
    GLint viewLoc = glGetUniformLocation(terraShader.getProgram(), "view");
    GLint projLoc =
        glGetUniformLocation(terraShader.getProgram(), "projection");

    // Pass the matrices to the shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(VAO);
    // Calculate the model matrix and pass it to shader before drawing
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));
    /* GLfloat angle = 20.0f * idx++; */
    /* model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f)); */
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    // Finally, draw terrain elements
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Swap the screen buffers
    glfwSwapBuffers(window_);
  }

  // TODO: refactor in cleanup-method
  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
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

void Game::initializeGlfw() {
  // Init GLFW
  glfwInit();
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
}

void Game::initializeGlew() {
  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();
}

void Game::initializeGl() {
  // Define the viewport dimensions
  glViewport(0, 0, Constants::WindowWidth, Constants::WindowHeight);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
