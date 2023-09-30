#pragma once

#include <utility>

#include "glm/glm.hpp"
#include "Ray.h"

namespace pbr
{
    struct BoundingBox
    {
        glm::vec3 Min;
        glm::vec3 Max;
    public:
        inline void Reset()
        {
            Min = glm::vec3(std::numeric_limits<float>::infinity());
            Max = -glm::vec3(std::numeric_limits<float>::infinity());
        }

        inline bool FindIntersection(const Ray& ray) const
        {
            glm::vec3 rayInv = 1.0f / ray.Direction;
            glm::vec3 bottomT = (Min - ray.Position) * rayInv;
            glm::vec3 topT = (Max - ray.Position) * rayInv;
            glm::vec3 minT = glm::min(topT, bottomT);
            glm::vec3 maxT = glm::max(topT, bottomT);

            float finalMinT = std::max(std::max(minT.x, minT.y), std::max(minT.x, minT.z));
            float finalMaxT = std::min(std::min(maxT.x, maxT.y), std::max(maxT.x, maxT.z));

            return finalMaxT > finalMinT;
        }
    };
}