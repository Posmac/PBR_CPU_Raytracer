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

        inline Ray(const glm::vec4& pos, const glm::vec4& dir, float dist)
            : Position(pos), Direction(dir), Distance(dist)
        {
        }

        glm::vec3 Position;
        glm::vec3 Direction;
        float Distance;
        glm::vec3 Color;
    };
}