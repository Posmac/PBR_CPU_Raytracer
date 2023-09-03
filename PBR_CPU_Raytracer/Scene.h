#pragma once

#include <algorithm>

#include "Camera.h"
#include "tinygltf/tiny_gltf.h"

namespace pbr
{
    class Scene
    {
    public:
        Scene();
        void Init(tinygltf::Model* model);
        void Render();
    private:
        Camera m_Camera;
    };
}