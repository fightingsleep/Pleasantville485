#ifndef CONTROLS_HPP
#define CONTROLS_HPP

extern bool renderDepthTexture;

void processUserInput();
void initCallbacks();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
#endif
