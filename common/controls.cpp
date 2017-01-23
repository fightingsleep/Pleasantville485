#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include "init.hpp"
#include "controls.hpp"
#include "window.hpp"

bool renderDepthTexture = false;

// Camera vectors
glm::vec3 position = glm::vec3(0, GROUND_Y_VALUE, -1);
glm::vec3 direction = glm::vec3(0, 0, 0.01);
glm::vec3 right = glm::vec3(-1, 0, 0);
glm::vec3 up = glm::cross(right, direction);

// Transform matrices
glm::mat4 ViewMatrix = glm::lookAt(
                                   // Camera is here
                                   position,
                                   // Camera looks here
                                   position + direction,
                                   // Vector pointing up from camera
                                   up
                                   );
glm::mat4 ProjectionMatrix = glm::perspective(45.0f,
                                              16.0f / 9.0f,
                                              0.1f,
                                              1000.0f);


glm::mat4 getViewMatrix()
{
    return ViewMatrix;
}

glm::mat4 getProjectionMatrix()
{
    return ProjectionMatrix;
}

// Mouse info
double xpos, ypos;
double mouseSpeed = 0.0015f;

// Camera movement speed (units / second)
float speed = 25.0f;

// Camera angles
double horizontalAngle = 0.0f;
double verticalAngle = 0.0f;

const float pi = 3.14;
bool godMode = false;
bool jumping = false;
bool falling = false;
bool hanging = false;

bool keys[1024];

void updateView() {
    ViewMatrix = glm::lookAt(
                             // Camera is here
                             position,
                             // Camera looks here
                             position + direction,
                             // Vector pointing up from camera
                             up
                             );
}

void processUserInput()
{
    static double lastTime = glfwGetTime();
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    double deltaTime = double(currentTime - lastTime);
    static int counter = 0;


    // Move forward
    if (keys[GLFW_KEY_W]){
        position += direction * float(deltaTime) * speed;
    }
    // Move backward
    if (keys[GLFW_KEY_S]){
        position -= direction * float(deltaTime) * speed;
    }
    // Strafe right
    if (keys[GLFW_KEY_D]){
        position += right * float(deltaTime) * speed;
    }
    // Strafe left
    if (keys[GLFW_KEY_A]){
        position -= right * float(deltaTime) * speed;
    }


    if (!godMode && !jumping && !falling && !hanging) {
        // Keep the camera on the x-z plane
        position = glm::vec3(position.x, GROUND_Y_VALUE, position.z);
    }

    if (jumping) {
        if (position.y >= 7.0) {
            jumping = false;
            hanging = true;
            counter = 0;
        } else if (position.y >= 5.0) {
            position = glm::vec3(position.x, position.y + 0.08, position.z);
        } else {
            position = glm::vec3(position.x, position.y + 0.1, position.z);
        }
    }

    if (hanging) {
        if (counter > 5) {
            hanging = false;
            falling = true;
            counter = 0;
        } else {
            counter++;
        }
    }

    if (falling) {
        position = glm::vec3(position.x, position.y - 0.1, position.z);
        if(position.y <= GROUND_Y_VALUE) {
            position = glm::vec3(position.x, GROUND_Y_VALUE, position.z);
            falling = false;
        }
    }

    // Update the View matrix
    updateView();

//    DEBUG PRINTING
//    printf("xpos: %f, ypos: %f\n", xpos, ypos);
//    printf("horizontalAngle: %f, verticalAngle: %f\n",
//           horizontalAngle, verticalAngle);
//
//
//    printf("double(xpos - lastXPos) = %f\n", double(xpos - lastXPos));
//    printf("double(lastYPos - ypos) = %f\n", double(lastYPos - ypos));
//
//    printf("mouseSpeed * double(xpos - lastXPos) = %f\n", mouseSpeed * double(xpos - lastXPos));
//    printf("mouseSpeed * double(lastYPos - ypos) = %f\n", mouseSpeed * double(lastYPos - ypos));
//
//    printf("\n\ndirection:(%f,%f,%f)\n,right:(%f,%f,%f)\n\n",
//           direction.x, direction.y, direction.z,
//           right.x, right.y, right.z);

    lastTime = currentTime;
}

void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
            keys[key] = true;
        else if (action == GLFW_RELEASE)
            keys[key] = false;
    }

    // Toggle godMode
    if (action == GLFW_PRESS && key == GLFW_KEY_G){
        godMode = !godMode;
    }
    // Toggle depth texture debug rendering
    if (action == GLFW_PRESS && key == GLFW_KEY_APOSTROPHE){
        renderDepthTexture = !renderDepthTexture;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE){
        if (!falling && !hanging && !godMode) {
            jumping = true;
        }
    }
}

void mouseMovementCallback(GLFWwindow* window, double xpos, double ypos)
{
    static double lastXPos, lastYPos;
    static bool firstTime = true;

    // Get mouse position
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstTime) {
        lastXPos = xpos;
        lastYPos = ypos;
        firstTime = false;
    }


    // Compute new orientation
    horizontalAngle += mouseSpeed * double(lastXPos - xpos);
    verticalAngle   += mouseSpeed * double(lastYPos - ypos);


    if (!godMode) {
        // Limit how far the camera can look up and down
        if (verticalAngle > pi/2) {
            verticalAngle = pi/2;
        } else if (verticalAngle < -pi/5) {
            verticalAngle = -pi/5;
        }
    }


    // Direction : Spherical coordinates to Cartesian coordinates conversion
    direction = glm::vec3(
                          cos(verticalAngle) * sin(horizontalAngle),
                          sin(verticalAngle),
                          cos(verticalAngle) * cos(horizontalAngle)
                          );

    // Right vector
    right = glm::vec3(
                      sin(horizontalAngle - pi/2.0f),
                      0,
                      cos(horizontalAngle - pi/2.0f)
                      );


    // Up vector
    up = glm::cross(right, direction);

    lastXPos = xpos;
    lastYPos = ypos;

    updateView();
}

void initCallbacks()
{
    glfwSetKeyCallback(window, keyPressCallback);
    glfwSetCursorPosCallback(window, mouseMovementCallback);
}

