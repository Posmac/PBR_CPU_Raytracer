#pragma once

#include <vector>
#include <algorithm>
#include <unordered_set>

#include "Camera.h"
#include "tinygltf/tiny_gltf.h"
#include "Model.h"
#include "GltfLoader.h"
#include "Light.h"
#include "Film.h"
#include "Globals.h"
#include "Ray.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtc/random.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/hash.hpp"

namespace pbr
{
    class Scene
    {
    public:
        Scene();
        void Init(tinygltf::Model* model, glm::ivec2* imageSize);
        void Render(Film* film);
        glm::mat4 GetNodeMatrix(tinygltf::Node* node);
        glm::vec4 GetPixelColor(float x, float y);
        void LoadNode(tinygltf::Model* model, tinygltf::Node* node, const glm::mat4& parentNodeMatrix, glm::ivec2* imageSize);
    private:
        std::vector<Camera> m_Cameras;
        std::vector<PointLight> m_PointLights;
        std::vector<Mesh> m_Meshes;
    };
}