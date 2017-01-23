#ifndef CONTROLS_HPP
#define CONTROLS_HPP

#define GROUND_Y_VALUE 0.3

extern bool renderDepthTexture;

void processUserInput();
void initCallbacks();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
#endif
