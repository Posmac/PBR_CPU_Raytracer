#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"

namespace pbr
{
    inline glm::vec3 RandomPointOnHemisphere(const glm::vec3& hemisphereNormal)
    {
        glm::vec3 rv = glm::sphericalRand(1.0f);
        if(glm::dot(rv, hemisphereNormal) < 0)
        {
            rv *= -1;
        }
        return rv;
    }
}