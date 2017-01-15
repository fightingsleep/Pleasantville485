//
//  physicsEngine.hpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-06.
//
//

#ifndef physicsEngine_hpp
#define physicsEngine_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <vector>

void simulatePhysics(std::vector<glm::mat4>& ModelMatrices, int numModels);

#endif /* physicsEngine_hpp */
