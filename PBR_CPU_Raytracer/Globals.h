#pragma once

#include <iostream>
#include <string>

#include "glm/glm.hpp"

namespace pbr
{
    const float WIDTH = 1920;
    const float HEIGHT = 1080;

    const glm::vec3 BLACK = glm::vec3(0, 0, 0);

    inline void LogInfo(const std::string& info)
    {
        std::cout << info << std::endl;
    }

    inline void LogVec2(const glm::vec2& v)
    {
        std::cout << v.x << ' ' << v.y << ' ' << '\n';
    }

    inline void LogVec3(const glm::vec3& v)
    {
        std::cout << v.x << ' ' << v.y << ' ' << v.z << '\n';
    }
}