//
//  init.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-02-05.
//
//

#include "init.hpp"
#include <GL/glew.h>
#include <glfw3.h>

/* A method for initializing our OpenGL context */
void initOpenGL()
{
    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);

    // Accept fragment if it is closer to the camera than the former one
    glDepthFunc(GL_LESS);

    // Cull back faces
//    glCullFace(GL_BACK);
//    glEnable(GL_CULL_FACE);

}





