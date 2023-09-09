#pragma once

#include <string>
#include <iostream>

#include "Core.h"
#include "Scene.h"

namespace pbr
{
    enum BufferType
    {
        ARRAY_BUFFER = 34962,
        ELEMENT_ARRAY_BUFFER = 34963,
    };

    enum AccessorType
    {
        SCALAR = 1,
        VEC2 = 2,
        VEC3 = 3,
    };

    enum ValueType
    {
        SIGNED_BYTE = 5120,
        UNSIGNED_BYTE = 5121,
        SIGNED_SHORT = 5122,
        UNSIGNED_SHORT = 5123,
        UNSIGNED_INT = 5125,
        FLOAT = 5126,
    };

    void LoadGltf(const std::string& path, tinygltf::Model* model);
}