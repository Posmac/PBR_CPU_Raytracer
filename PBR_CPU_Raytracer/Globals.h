#pragma once

#include <iostream>
#include <string>

namespace pbr
{
    const float HEIGHT = 108 * 2.0;
    const float WIDTH = 144 * 2.0;

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