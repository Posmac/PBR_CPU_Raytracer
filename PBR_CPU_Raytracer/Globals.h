#pragma once

#include <iostream>
#include <string>
#include <cmath>

#include "glm/glm.hpp"

namespace pbr
{
    const float WIDTH = 720;
    const float HEIGHT = 540;
    const int SAMPLES_PER_PIXEL = 256;
    const int SAMPLER_SQRT = static_cast<int>(std::ceil(std::sqrt(SAMPLES_PER_PIXEL)));

    const int PATH_DEPTH = 10;
    const bool USE_RUS_RUL = false;
    const int MIN_RUS_RUL_THRASHOLD = 5;

    /*const float DEFUCUS_ANGLE = 0;
    const float FOCUS_DIST = 10;
    const glm::vec3 DEFOCUS_DISK_U;
    const glm::vec3 DEFOCUS_DISK_V;*/

    const glm::vec3 BLACK = glm::vec3(0);
    const glm::vec3 GRAY = glm::vec3(64);

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