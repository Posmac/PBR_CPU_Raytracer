#pragma once

#include <vector>
#include <algorithm>

#include "Camera.h"
#include "Globals.h"
#include "tinygltf/tiny_gltf.h"
#include "Model.h"
#include "GltfLoader.h"
#include "Light.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace pbr
{
    class Scene
    {
    public:
        Scene();
        void Init(tinygltf::Model* model, float height = HEIGHT);
        void Render();
        glm::mat4 GetNodeMatrix(tinygltf::Node* node);
    private:
        std::vector<Camera> m_Cameras;
        std::vector<PointLight> m_PointLights;
        std::vector<Mesh> m_Meshes;
    };
}