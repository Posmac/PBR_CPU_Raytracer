#pragma once

#include "glm/glm.hpp"

namespace pbr
{
    struct Ray
    {
        inline Ray()
        {
            Distance = std::numeric_limits<float>::max();
        }

        glm::vec3 Direction;
        glm::vec3 Position;
        glm::vec3 Color;

        float Distance;
    };
}