//
//  PhysicsEntity.hpp
//  CMPT485
//
//  Created by Chris Ross on 2016-06-09.
//
//

#ifndef PhysicsEntity_hpp
#define PhysicsEntity_hpp

#include <stdio.h>
#include <glm/glm.hpp>

class PhysicsEntity
{
private:
    glm::vec3 position;
    glm::vec3 force;
    glm::vec3 gravity = glm::vec3(0, -9.8, 0);
    glm::mat4 model;

public:
    PhysicsEntity(glm::vec3 pos = glm::vec3(0,0,0),
                  glm::mat4 M = glm::mat4(0)) :
    position(pos), model(M)
    {
    }

    void ApplyForce(glm::vec3 f);
    void setGravity(glm::vec3 grav) {
        gravity = grav;
    }
    glm::vec3 getGravity() {
        return gravity;
    }
    void setModel(glm::mat4 M) {
        model = M;
    }

    glm::mat4 getModel() {
        return model;
    }

};

#endif /* PhysicsEntity_hpp */
