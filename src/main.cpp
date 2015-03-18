#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

#include <GL/glew.h>

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "terrain.h"
#include "constants.h"
#include "camera.h"

// Functions
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void do_movement(const GLfloat &deltaTime);

// Camera
Camera camera(glm::vec3(Constants::TileWidth / 2, 60.0f,
                        Constants::TileWidth / 2));

// Input keys
bool keys[1024];


int main() {

  // INITALIZE
  ////////////
  // TODO: create Init-methods

  // Init GLFW
  glfwInit();
  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  // Create a GLFWwindow object that we can use for GLFW's functions
  GLFWwindow *window =
      glfwCreateWindow(Constants::WindowWidth, Constants::WindowHeight,
                       "litlanesfoss", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  // Set the required callback functions
  glfwSetKeyCallback(window, key_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);

  // GLFW Options
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // Set this to true so GLEW knows to use a modern approach to retrieving
  // function pointers and extensions
  glewExperimental = GL_TRUE;
  // Initialize GLEW to setup the OpenGL Function pointers
  glewInit();

  // Define the viewport dimensions
  glViewport(0, 0, Constants::WindowWidth, Constants::WindowHeight);

  glEnable(GL_DEPTH_TEST);

  // CREATE STUFF
  ///////////////

  // Build and compile our shader program
  Shader terraShader("shader/default.vert", "shader/default.frag");

  // Set up vertex data (and buffer(s)) and attribute pointers
  // Create vertices of mesh of 2^Constants::TileWidth
  // Defaults to Perlin noise
  Terrain terrain;
  terrain.create();
  auto vertices = terrain.getVertices();
  auto indices = terrain.getIndices();

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

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // Deltatime
  GLfloat deltaTime = 0.0f; // Time between current frame and last frame
  GLfloat lastFrame = 0.0f; // Time of last frame

  // Game loop
  while (!glfwWindowShouldClose(window)) {
    // TODO: refactor in processInput(), update(), render()
    // Calculate deltatime of current frame
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Check if any events have been activiated (key pressed, mouse moved etc.)
    // and call corresponding response functions
    glfwPollEvents();
    do_movement(deltaTime);

    // Render
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activate shader
    terraShader.use();

    // Camera/View transformation
    glm::mat4 view;
    view = camera.getViewMatrix();

    // Projection
    glm::mat4 projection;
    projection = glm::perspective(
        camera.getZoom(), static_cast<GLfloat>(Constants::WindowWidth) /
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
    glfwSwapBuffers(window);
  }

  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow *window, int key, int scancode, int action,
                  int mode) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);
  if (key >= 0 && key < 1024) {
    if (action == GLFW_PRESS) {
      keys[key] = true;
    } else if (action == GLFW_RELEASE) {
      keys[key] = false;
    }
  }
}

void do_movement(const GLfloat &deltaTime) {
  // Camera keyboard controls
  if (keys[GLFW_KEY_W]) {
    camera.processKeyboard(Camera::FORWARD, deltaTime);
  }
  if (keys[GLFW_KEY_S]) {
    camera.processKeyboard(Camera::BACKWARD, deltaTime);
  }
  if (keys[GLFW_KEY_A]) {
    camera.processKeyboard(Camera::LEFT, deltaTime);
  }
  if (keys[GLFW_KEY_D]) {
    camera.processKeyboard(Camera::RIGHT, deltaTime);
  }
  if (keys[GLFW_KEY_E]) {
    camera.processKeyboard(Camera::UP, deltaTime);
  }
  if (keys[GLFW_KEY_Q]) {
    camera.processKeyboard(Camera::DOWN, deltaTime);
  }
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  static bool firstMouse = true;
  static GLfloat lastX;
  static GLfloat lastY;

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

  camera.processMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.processMouseScroll(static_cast<GLfloat>(yoffset));
}
