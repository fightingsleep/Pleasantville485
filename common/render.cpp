//
//  render.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-13.
//
//

#include "render.hpp"
#include "shader.hpp"
#include <vector>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

GLuint skyboxShaderID;
GLuint cubemapTexture;
GLuint skyboxVAO;

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width,height;
    unsigned char* image;

    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
        SOIL_free_image_data(image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return textureID;
}

void initSkybox()
{
    skyboxShaderID = LoadShaders("../src/skybox.vs", "../src/skybox.frag");
    GLuint skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glBindVertexArray(0);

    std::vector<const GLchar*> faces;
    faces.push_back("../src/buildings/skybox/right.jpg");
    faces.push_back("../src/buildings/skybox/left.jpg");
    faces.push_back("../src/buildings/skybox/top.jpg");
    faces.push_back("../src/buildings/skybox/bottom.jpg");
    faces.push_back("../src/buildings/skybox/back.jpg");
    faces.push_back("../src/buildings/skybox/front.jpg");
    cubemapTexture = loadCubemap(faces);

}

void renderSkybox(glm::mat4 view, glm::mat4 projection)
{
    // Draw skybox as last
    glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
    glUseProgram(skyboxShaderID);
    view = glm::mat4(glm::mat3(view));	// Remove any translation component of the view matrix
    glUniformMatrix4fv(glGetUniformLocation(skyboxShaderID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShaderID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    // skybox cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(skyboxShaderID, "skybox"), 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS); // Set depth function back to default
}
