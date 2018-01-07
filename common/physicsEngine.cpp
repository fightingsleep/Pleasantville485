//
//  physicsEngine.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-06.
//
//

#include "physicsEngine.hpp"
#include "physicsEntity.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <vector>

bool initialized = false;
PhysicsEntity *entities;

void simulatePhysics(std::vector<glm::mat4>& ModelMatrices, int numModels)
{

    if (!initialized) {
        entities = new PhysicsEntity[numModels];
        for (int i = 0; i < numModels; i++) {
            entities[i].setModel(ModelMatrices[i]);
        }
        initialized = true;
    }


    for (int i = 0; i < numModels; i++) {
        entities[i].ApplyForce(glm::vec3(0.0000001,0,0));
        ModelMatrices[i] = entities[i].getModel();
    }
}
