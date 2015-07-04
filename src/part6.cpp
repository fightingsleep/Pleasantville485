// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/texture.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

float cubicSpline(float p1, float p1_prime, float p2, float p2_prime, float t)
{
  float tmp[16] = {1, 0, -3, 2, 0, 1, -2, 1, 0, 0, 3, -2, 0, 0, -1, 1};
  glm::mat4 c_inv = glm::make_mat4(tmp);
  glm::vec4 a = glm::vec4(p1, p1_prime, p2, p2_prime);
  glm::vec4 coeffs = c_inv * a;
  float result = coeffs.x + (coeffs.y * t) + (coeffs.z * pow(t,2)) + (coeffs.w * pow(t, 3));
  return result;
}

int main(int argc, char **argv)
{ 
    /**********************************/
    /*** APPLICATION INITIALIZATION ***/
    /**********************************/

    if(argc != 2)
    {
      printf("Usage: ./part6 example.campus\n");
      exit(1);
    }
    
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1920, 1080, "CMPT 485", NULL, NULL);
    if( window == NULL ){
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
    
    /**********************************/
    /***** OPENGL INITIALIZATION ******/
    /**********************************/
  
    // Dark blue background
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );
    // Use our shader
    glUseProgram(programID);
    
    // Get a handle for our "MVP" uniform
    GLuint ViewProjectionMatrixID = glGetUniformLocation(programID, "VP");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    
    GLuint shininessID = glGetUniformLocation(programID, "shininess");
    GLuint ambientProductID = glGetUniformLocation(programID, "ambientProduct");
    GLuint diffuseProductID = glGetUniformLocation(programID, "diffuseProduct");
    GLuint specularProductID = glGetUniformLocation(programID, "specularProduct");
    GLuint lightID = glGetUniformLocation(programID, "lightPosition");
    GLuint cameraID = glGetUniformLocation(programID, "cameraPosition");

    vec4 ambientProduct = vec4(0.80, 0.80, 0.80, 1);
    vec4 diffuseProduct = vec4(0.1, 0.1, 0.1, 1);
    vec4 specularProduct = vec4(0.1, 0.1, 0.1, 1);
    vec4 lightPosition = vec4(0, 200, 0, 1);
    vec4 cameraPosition = getCurrCameraPosition();

    glUniform1f(shininessID, 1);
    glUniform4fv(ambientProductID, 1, &ambientProduct[0]);
    glUniform4fv(diffuseProductID, 1, &diffuseProduct[0]);
    glUniform4fv(specularProductID, 1, &specularProduct[0]);
    glUniform4fv(lightID, 1, &lightPosition[0]);
    glUniform4fv(cameraID, 1, &cameraPosition[0]);
    
    // Initialize GLFW control callbacks
    initializeControls();
    
    // Projection and Model matrices are fixed
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    
    /**********************************/
    /**** LOAD MODEL INFORMATION ******/
    /**********************************/

    std::vector<Building> buildings;
    char campus_name[1024];
    loadBuildings(argv[1], campus_name, buildings);
    for (int i  = 0; i < buildings.size(); i++) {
        printf("Found %s \n", buildings[i].modelsFilename.c_str());
        printf("Txyz = %f, %f, %f\n", buildings[i].tx, buildings[i].ty, buildings[i].tz);
    }
     
    char fileName[1024];
    /**********************************/
    /*** SET MODEL MATRIX FROM INFO ***/
    /**********************************/
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    int numModels = buildings.size();
    
    GLuint vaoIDs[numModels];
    GLsizei numVertices[numModels];
    GLsizei numVertexIndices[numModels];
    GLuint textures[numModels];
    std::vector<glm::mat4> ModelMatrix;
    ModelMatrix.resize(numModels);
    
    glGenVertexArrays(numModels, vaoIDs);
    
    for (int i  = 0; i < numModels; i++)
    {    
        // Read our .obj file
        std::vector<Model> models;
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> uvs;
        std::vector<glm::vec3> normals;
        std::vector<glm::ivec3> vertex_indices;
        std::vector<glm::ivec3> uv_indices;
        std::vector<glm::ivec3> normal_indices;
 
        char *tmp = (char *)malloc(256 * sizeof(char));
        char *p1 = (char *)malloc(256 * sizeof(char));
        char *p2 = (char *)malloc(256 * sizeof(char));
        char *path = (char *)malloc(256 * sizeof(char));
        strcpy(tmp, (char *)buildings[i].modelsFilename.c_str());
        p1 = strtok(tmp, "/");
        p2 = strtok(NULL, "/");
        strcpy(path,p1);
        strcat(path,"/");
        strcat(path, p2);

        loadModels((char *)buildings[i].modelsFilename.c_str(), models); 

        sprintf(fileName, "%s/%s.obj", path, p2);
        bool loadSuccess = loadOBJ(fileName, vertices, uvs, normals);
        if (!loadSuccess) {
            return -1;
        } 

        // need to keep track of vbo sizes for drawing later
        numVertices[i] = vertices.size();
        numVertexIndices[i] = vertex_indices.size();
        
        ModelMatrix[i] = glm::mat4(1.0f);
        ModelMatrix[i] = glm::scale(ModelMatrix[i], glm::vec3(models[0].sx, models[0].sy, models[0].sz));
        ModelMatrix[i] = glm::rotate(ModelMatrix[i], models[0].ra,glm::vec3(models[0].rx, models[0].ry, models[0].rz));
        ModelMatrix[i] = glm::translate(ModelMatrix[i], glm::vec3(models[0].tx, models[0].ty, models[0].tz));
        
        ModelMatrix[i] = glm::scale(ModelMatrix[i], glm::vec3(buildings[i].sx, buildings[i].sy, buildings[i].sz));
        ModelMatrix[i] = glm::rotate(ModelMatrix[i], buildings[i].ra,glm::vec3(buildings[i].rx, buildings[i].ry, buildings[i].rz));
        ModelMatrix[i] = glm::translate(ModelMatrix[i], glm::vec3(buildings[i].ox, buildings[i].oy, buildings[i].oz));
        ModelMatrix[i] = glm::translate(ModelMatrix[i], glm::vec3(buildings[i].tx, buildings[i].ty, buildings[i].tz));

        // Load vertices into a VBO
        GLuint vertexbuffer;
        glGenBuffers(1, &vertexbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glBufferData(GL_ARRAY_BUFFER, numVertices[i] * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        
        // Load normals into a VBO
        GLuint normalsbuffer;
        glGenBuffers(1, &normalsbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
        
        // Load texture coords into a VBO
        GLuint uvbuffer;
        glGenBuffers(1, &uvbuffer);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec3), &uvs[0], GL_STATIC_DRAW);
        
        // bind VAO in order to save attribute state
        glBindVertexArray(vaoIDs[i]);
        
        sprintf(fileName, "%s/%s.bmp", path, p2);
        
        GLuint t = loadBMP_custom(fileName);
        textures[i] = t;
        
        // Set our "myTextureSampler" sampler to user Texture Unit i
        glUniform1i(TextureID, i);
        
        // 1st attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                              0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 2nd attribute buffer : normals
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, normalsbuffer);
        glVertexAttribPointer(
                              1,                  // attribute. No particular reason for 1, but must match the layout in the shader.
                              3,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
        
        // 3rd attribute buffer : uvs
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
                              2,                  // attribute. No particular reason for 2, but must match the layout in the shader.
                              2,                  // size
                              GL_FLOAT,           // type
                              GL_FALSE,           // normalized?
                              0,                  // stride
                              (void*)0            // array buffer offset
                              );
           
        /**********************************/
        /*** UNBIND VERTEX-ARRAY OBJECT ***/
        /**********************************/
        glBindVertexArray(0);
    }

    glm::mat4 ViewMatrix;
                         
                         
    float t = 0.0;
    int counter = 0;
    int frameCounter = 0;
    glm::vec3 previousEye = glm::vec3(-8.440007, 3.306859, -13.259031);
    glm::vec3 previousDir = glm::vec3(-0.996292, -0.082713, 0.024487);
    glm::vec3 previousUp = glm::vec3(0.0, 1.0, 0.0);

    glm::vec3 currEye = glm::vec3(-16.908274, 5.235496, -3.581913);
    glm::vec3 currDir = glm::vec3(0.945258, -0.258318, 0.199582);
    glm::vec3 currUp = glm::vec3(0.0, 1.0, 0.0);
  
    do
    {
      if(t >= 1.0 && counter == 0)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
	currEye = glm::vec3(-12.176553, 5.196604, 9.659609);
	currDir = glm::vec3(0.603095, -0.417664, -0.679642);
	currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 1)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
	currEye = glm::vec3(-8.463412, 3.405194, 16.408813);
	currDir = glm::vec3(0.404287, -0.155523, -0.901362);
	currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 2)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
        currEye = glm::vec3(10.907748, 3.674664, 17.116156);
        currDir = glm::vec3(-0.349221, -0.094564, -0.932312);
        currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 3)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
        currEye = glm::vec3(16.134892, 1.820505, -15.305500);
        currDir = glm::vec3(-0.989909, -0.022327, 0.140322);
        currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 4)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
        currEye = glm::vec3(3.817585, 1.049228, -23.092480);
        currDir = glm::vec3(-0.728911, -0.049851, 0.682875);
        currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 5)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;
        currEye = glm::vec3(-10.692534, 4.235083, -16.644449);
        currDir = glm::vec3(0.382175, -0.213521, 0.899151);
        currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 6)
      {
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;

        currEye = glm::vec3(-16.908274, 5.235496, -3.581913);
        currDir = glm::vec3(0.945258, -0.258318, 0.199582);
        currUp = glm::vec3(0.0, 1.0, 0.0);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 7)
      { 
        previousEye = currEye;
        previousDir = currDir;
        previousUp = currUp;

        currEye = glm::vec3(-23.863096, 38.834099, -1.383326);
        currDir = glm::vec3(0.656888, -0.754009, -0.000984);
        currUp = glm::vec3(0.753999, 0.656881, 0.003755);
        counter++;
        t = 0.0;
      }
      else if(t >= 1.0 && counter == 8)
      {
        return 0;
      }
      else{}

        // Get updated view matrix (mouse input handled with callback functions)
        //ViewMatrix = getViewMatrix();
        //cameraPosition = getCurrCameraPosition();
        cameraPosition = vec4(cubicSpline(previousEye.x, 0,currEye.x,0,t),
                              cubicSpline(previousEye.y, 0,currEye.y,0,t),
                              cubicSpline(previousEye.z, 0,currEye.z,0,t), 1);
        glUniform4fv(cameraID, 1, &cameraPosition[0]);

        ViewMatrix = glm::lookAt(
                            glm::vec3(
                              cubicSpline(previousEye.x, 0,currEye.x,0,t),
                              cubicSpline(previousEye.y, 0,currEye.y,0,t),
                              cubicSpline(previousEye.z, 0,currEye.z,0,t)
                            ),
                            glm::vec3(
                              cubicSpline(previousDir.x, 0,currDir.x,0,t),
                              cubicSpline(previousDir.y, 0,currDir.y,0,t),
                              cubicSpline(previousDir.z, 0,currDir.z,0,t)
                            ),
                            glm::vec3(
                              cubicSpline(previousUp.x, 0,currUp.x,0,t),
                              cubicSpline(previousUp.y, 0,currUp.y,0,t),
                              cubicSpline(previousUp.z, 0,currUp.z,0,t)
                            )
        );

	t = t + 0.008;
        glm::mat4 VP = ProjectionMatrix * ViewMatrix;
        
        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(ViewProjectionMatrixID, 1, GL_FALSE, &VP[0][0]);
        
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        /**********************************/
        /************** DRAW  *************/
        /**********************************/ 
        for (int i = 0; i < numModels; i++)
        {    
            // bind VAO to recall VBOs state
            glBindVertexArray(vaoIDs[i]);
            glActiveTexture(i);
            glBindTexture(GL_TEXTURE_2D, textures[i]);
            // Set our "myTextureSampler" sampler to user Texture Unit 0
            glUniform1i(TextureID, 0);
            
            // Set our Model transform matrix
            glm::mat4 M = ModelMatrix[i];
            glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &M[0][0]);
            
            glDrawArrays(GL_TRIANGLES, 0, numVertices[i] );

            //ModelMatrix[i] = glm::rotate(ModelMatrix[i], 1.0f, vec3(0,1,0));
            
            // Unbind VAO
            glBindVertexArray(0);
        }

        /*
        unsigned char data[1280 * 720 * 3];
        unsigned char flippedData[1280 * 720 * 3];
        glReadPixels(0, 0, 1280, 720, GL_RGB, GL_UNSIGNED_BYTE, data);

        for(int i = 0; i < 720; i++)
        {
          memcpy(flippedData + (i * (1280 * 3)), data + ((719 - i) * (1280 * 3)), 1280 * 3);
        }

        FILE * file;
        char filename[256];
        sprintf(filename, "/run/media/clr446/USBSTICK/frames/frame%d.ppm", frameCounter);
        file = fopen (filename, "w");
        fprintf(file, "P6\n1280 720\n255\n");
        fwrite (flippedData , sizeof(unsigned char), sizeof(flippedData), file);
        fclose (file);
        frameCounter++;
        */
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
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

