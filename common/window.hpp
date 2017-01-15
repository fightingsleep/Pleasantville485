//
//  window.hpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-03.
//
//

#ifndef window_hpp
#define window_hpp

#include <stdio.h>
#include <GL/glew.h>
#include <glfw3.h>

/* The display window in which the application will run */
extern GLFWwindow* window;

int initDisplayWindow();

#endif /* window_hpp */
