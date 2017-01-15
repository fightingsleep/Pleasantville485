//
//  window.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-03.
//
//

#include <GL/glew.h>
#include <glfw3.h>
#include "window.hpp"

GLFWwindow* window;

/* A method for initializing the display window */
int initDisplayWindow()
{

    // Initialise GLFW
    if (!glfwInit()) {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        glfwTerminate();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 2560, 1440, "CMPT 485", NULL, NULL);
    if ( window == NULL ) {
        fprintf( stderr, "Failed to open GLFW window.\n" );
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Hide the cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSwapInterval(1);

    return 0;
}