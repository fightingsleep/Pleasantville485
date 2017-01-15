//
//  PhysicsEntity.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-09.
//
//

#include "PhysicsEntity.hpp"
#include <glm/gtc/matrix_transform.hpp>

#define SUCCESS 0

void PhysicsEntity::ApplyForce(glm::vec3 f)
{
    glm::vec3 computedForce = gravity + f;

    position = position + computedForce;

    if (position.y < 0.0) {
        position.y = 0.0;
    }

    model = glm::translate(model, position);

}