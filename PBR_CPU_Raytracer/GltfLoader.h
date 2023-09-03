#pragma once

#include <string>
#include <iostream>

#include "Core.h"
#include "Scene.h"

namespace pbr
{
    void LoadGltf(const std::string& path, tinygltf::Model* model);
}