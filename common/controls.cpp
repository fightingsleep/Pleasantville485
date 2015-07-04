// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
using namespace glm;

#include "controls.hpp"

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

// Initial distance between eye and center :
static float radius = 3;

// Initial positions and directions of view :
glm::vec4 center = glm::vec4( 0, 0, 0, 1);
glm::vec4 direction = glm::vec4( 0, 0, -1, 0);
glm::vec4 right = glm::vec4( 1, 0, 0, 0);
glm::vec4 up = glm::vec4( 0, 1, 0, 0 );

glm::vec4 getCurrCameraPosition(){
        return center;
}

// Mouse info
double xpos, ypos;
float dx, dy;
bool rotating = false;
bool translating = false;

float translationSpeed = 0.005f;
float rotationSpeed = 0.2f;
float zoomSpeed = 0.01f;

void updateView() {
    glm::vec3 camPos = glm::vec3(center-radius*direction);
    printf("\n\ncurrEye = glm::vec3(%f, %f, %f);\n", camPos.x, camPos.y, camPos.z);
    printf("currDir = glm::vec3(%f, %f, %f);\n", direction.x, direction.y, direction.z);
    printf("currUp = glm::vec3(%f, %f, %f);\n\n", up.x, up.y, up.z);
    ViewMatrix = glm::lookAt(
                             glm::vec3(center-radius*direction),           // Camera is here
                             glm::vec3(center), // and looks here : at the same position, plus "direction"
                             glm::vec3(up)                  // Head is up (set to 0,-1,0 to look upside-down)
                             );
}

void MouseDraggedCallback(GLFWwindow*, double x, double y)
{
    dx = x - xpos; // x cursor increases from left to right
    dy = ypos - y; // y cursor increases from top to bottom
    
    if (rotating)
    {
//        printf ("rotate: dx = %f, dy = %f\n", dx, dy);

        glm::mat4 Rtilt = glm::rotate(glm::mat4(1.0f), rotationSpeed * dy, glm::vec3(right));
        direction = Rtilt * direction;
        up = Rtilt * up;

        glm::mat4 Rpan = glm::rotate(glm::mat4(1.0f), rotationSpeed * -dx, glm::vec3(up));
        direction = Rpan * direction;
        right = Rpan * right;

        updateView();
    }
    
    if (translating)
    {
//        printf ("translate: dx = %f, dy = %f\n", dx, dy);
        
        center -= dx * translationSpeed * right;
        center -= dy * translationSpeed * up;
        updateView();
    }
    
    xpos = x;
    ypos = y;
}


void MouseScrollCallback(GLFWwindow*, double dx, double dy)
{
    radius += zoomSpeed * radius * dy;
    updateView();
}

void MousePressCallback(GLFWwindow*, int button, int action, int mods)
{
    glfwGetCursorPos(window, &xpos, &ypos);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        rotating = true;
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        translating = true;
    }

    if (action == GLFW_RELEASE)
    {
        rotating = false;
        translating = false;
    }
}

void initializeControls() {
    
    // Setup callback functions
    glfwSetScrollCallback(window, MouseScrollCallback);
    glfwSetMouseButtonCallback(window, MousePressCallback);
    glfwSetCursorPosCallback(window, MouseDraggedCallback);
    
    // Initialize View and Project matrices
    ProjectionMatrix = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 200.0f);
    updateView();
    
}

