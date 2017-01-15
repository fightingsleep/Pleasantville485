//
//  cubicSpline.cpp
//  CMPT485
//
//  Created by Chris Ross on 2016-02-05.
//
//

#include "cubicSpline.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

float cubicSpline(float p1, float p1_prime, float p2, float p2_prime, float t)
{
    float tmp[16] = {1, 0, -3, 2, 0, 1, -2, 1, 0, 0, 3, -2, 0, 0, -1, 1};
    glm::mat4 c_inv = glm::make_mat4(tmp);
    glm::vec4 a = glm::vec4(p1, p1_prime, p2, p2_prime);
    glm::vec4 coeffs = c_inv * a;
    float result = coeffs.x + (coeffs.y * t) + (coeffs.z * pow(t,2)) +
                   (coeffs.w * pow(t, 3));
    
    return result;
}