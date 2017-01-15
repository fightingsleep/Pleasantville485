#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>
#include <stdio.h>
#include <common/init.hpp>
#include <common/window.hpp>
#include <common/physicsEngine.hpp>
#include <common/debug.hpp>
#include <common/render.hpp>

int main(int argc, char **argv)
{
    
    if (2 != argc) {
        printf("Usage: ./part6 example.campus\n");
        exit(1);
    }
    
    if (0 != initDisplayWindow()) {
        printf("Display window initilization failed\n");
        exit(1);
    }

    initOpenGL();
    
    // Create and compile our GLSL program from the shaders
    GLuint depthProgramID = LoadShaders("DepthTest.vertexshader",
                                        "DepthTest.fragmentshader");
    GLuint programID = LoadShaders("ShadowMapping.vertexshader",
                                   "ShadowMapping.fragmentshader");
    
    // Getting handles for all uniform variables
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ProjectionMatrixID = glGetUniformLocation(programID, "P");
    GLuint depthMatrixID = glGetUniformLocation(depthProgramID, "depthMVP");
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    GLuint DepthBiasID = glGetUniformLocation(programID, "DepthBiasMVP");
    GLuint ShadowMapID = glGetUniformLocation(programID, "shadowMap");
    GLuint lightInvDirID = glGetUniformLocation(programID,
                                                "LightInvDirection_worldspace");
    

    glm::mat4 M; // Model Matrix
    glm::mat4 V = getViewMatrix(); // View Matrix
    glm::mat4 P = getProjectionMatrix(); // Projection matrix is fixed
    
    /**********************************/
    /**** LOAD MODEL INFORMATION ******/
    /**********************************/
    
    std::vector<Building> buildings;
    char campus_name[1024];
    loadBuildings(argv[1], campus_name, buildings);
    for (int i  = 0; i < buildings.size(); i++) {
        printf("Found %s \n", buildings[i].modelsFilename.c_str());
        printf("Txyz = %f, %f, %f\n",
               buildings[i].tx,
               buildings[i].ty,
               buildings[i].tz);
    }
    
    int numModels = buildings.size();
    GLuint *vaoIDs;
    std::vector<glm::mat4> ModelMatrices;
    GLuint *textures;
    GLsizei *numVertices;
    bool retVal = loadAllBuildingsIntoVAOs(buildings,
                                           &vaoIDs,
                                           &textures,
                                           ModelMatrices,
                                           &numVertices);
    if (false == retVal) {
        printf("VAO initialization failed\n");
        exit(1);
    }
    
    // DEPTH TEXTURE STUFF BEGINS HERE (for shadow mapping)
    GLuint FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    GLuint depthTexture;
    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 2560, 1440, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_COMPARE_MODE,
                    GL_COMPARE_R_TO_TEXTURE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    
    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    
    // Always check that our framebuffer is ok
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        return false;
    }

    glm::mat4 depthBiasMVP;
    glm::mat4 biasMatrix(
        0.5, 0.0, 0.0, 0.0,
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    );

    // Set the position of the sun
    glm::vec3 lightInvDir = glm::vec3(-7, 10, -10);
    glUniform3f(lightInvDirID, lightInvDir.x, lightInvDir.y, lightInvDir.z);

    // Compute the MVP matrix from the light's point of view
    glm::mat4 depthProjectionMatrix = glm::ortho<float>(-60, 60, -60,
                                                        60, -50, 100);
    glm::mat4 depthModelMatrix = glm::mat4(1.0);

    initSkybox();
    initCallbacks();


    // Game loop
    do {
        
        glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir,
                                                glm::vec3(0,0,0),
                                                glm::cross(glm::vec3(0,0,1),
                                                           glm::vec3(
                                                               -lightInvDir)));
        glm::mat4 depthMVP = depthProjectionMatrix *
        depthViewMatrix *
        depthModelMatrix;

        // Process user input
        processUserInput();

        // Simulate physics
        //simulatePhysics(ModelMatrices, numModels);
        
        // Render to our depth framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
        glViewport(0,0,2560,1440);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(depthProgramID);
        
        /**********************************************/
        /************** DRAW DEPTH BUFFER *************/
        /**********************************************/
        for (int i = 0; i < numModels; i++) {

            depthModelMatrix = ModelMatrices[i];
            depthMVP = depthProjectionMatrix *
                       depthViewMatrix *
                       depthModelMatrix;
            glUniformMatrix4fv(depthMatrixID, 1, GL_FALSE, &depthMVP[0][0]);
            
            glBindVertexArray(vaoIDs[i]);
            
            glDrawArrays(GL_TRIANGLES, 0, numVertices[i]);
            
            glBindVertexArray(0);
        }
        
        
        // Render to the default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0,0,2560,1440);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(programID);
        
        V = getViewMatrix();
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &V[0][0]);
        
        /**********************************/
        /************** DRAW  *************/
        /**********************************/
        for (int i = 0; i < numModels; i++) {
            
            glActiveTexture(GL_TEXTURE0+2);
            glBindTexture(GL_TEXTURE_2D, depthTexture);
            glUniform1i(ShadowMapID, 2);
            
            glActiveTexture(GL_TEXTURE0+1);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            glUniform1i(TextureID, 1);

            M = ModelMatrices[i];
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);
            glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(ProjectionMatrixID, 1, GL_FALSE, &P[0][0]);
            
            depthMVP = depthProjectionMatrix * depthViewMatrix * M;
            depthBiasMVP = biasMatrix * depthMVP;
            glUniformMatrix4fv(DepthBiasID, 1, GL_FALSE, &depthBiasMVP[0][0]);
            
            glBindVertexArray(vaoIDs[i]);
            
            glDrawArrays(GL_TRIANGLES, 0, numVertices[i]);
            
            glBindVertexArray(0);
        }

        if (renderDepthTexture) {
            renderDepthBufferToScreen(depthTexture);
        }

        renderSkybox(V, P);


        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup VBO and shader
    glDeleteProgram(programID);
    for (int i = 0; i < numModels; i++) {
        glDeleteVertexArrays(1, &vaoIDs[i]);
    }
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

