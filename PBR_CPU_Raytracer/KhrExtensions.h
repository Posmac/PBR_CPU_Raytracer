#pragma once

#include "glm/glm.hpp"

namespace pbr
{
    //"KHR_materials_transmission"
    struct KHR_materials_transmission
    {
        int transmissionKHR = -1;
        int transmissionFactor = -1;
    };
  
    //"KHR_materials_volume"
    struct KHR_materials_volume
    {
        int volumeKHR = -1;
        int volumeThicknessFactor;
        glm::vec3 volumeAttenuationColor;
    };
}