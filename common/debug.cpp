//
//  debug.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-02.
//
//

#include "debug.hpp"
#include <common/shader.hpp>

static const GLfloat g_quad_vertex_buffer_data[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
};

GLuint quad_programID = NULL;
GLuint texID = NULL;
GLuint quad_vao = NULL;
GLuint quad_vertexbuffer = NULL;

/* A method for initializing a debug window on screen */
void initDebugRendering()
{
    quad_programID = LoadShaders( "Passthrough.vertexshader", "SimpleTexture.fragmentshader" );
    texID = glGetUniformLocation(quad_programID, "texture");

    glGenVertexArrays(1, &quad_vao);
    glBindVertexArray(quad_vao);

    glGenBuffers(1, &quad_vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_quad_vertex_buffer_data), g_quad_vertex_buffer_data, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, quad_vertexbuffer);
    glVertexAttribPointer(
                          0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    glBindVertexArray(0);
}

/* A method for rendering our debug window to the screen */
void renderDepthBufferToScreen(GLuint depthTexture)
{
    static bool initialized = false;

    if (!initialized) {
        initDebugRendering();
        initialized = true;
    }

    glViewport(0,0,512,512);
    glUseProgram(quad_programID);
    glActiveTexture(GL_TEXTURE0+7);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(texID, 7);
    glBindVertexArray(quad_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
    glBindVertexArray(0);
}