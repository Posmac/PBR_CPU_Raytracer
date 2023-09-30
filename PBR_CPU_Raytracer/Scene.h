#pragma once

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <fstream>
#include <map>
#include <set>

#include "Camera.h"
#include "tinygltf/tiny_gltf.h"
#include "Model.h"
#include "GltfLoader.h"
#include "Light.h"
#include "Film.h"
#include "Globals.h"
#include "Ray.h"

#include "glm/glm.hpp"
#include "glm/gtc/random.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "glm/gtc/type_ptr.hpp"
#include <glm/gtx/quaternion.hpp>
#include "glm/gtx/hash.hpp"

#include "RenderData.h"

namespace pbr
{
    struct Node
    {
        Node* NextNode;
        int InstanceId = -1;
    };

    class Scene
    {
    public:
        Scene();
        void Init(tinygltf::Model* model, glm::ivec2* imageSize,
                  const std::string& modelPath);
        void Render(Film* film);
        glm::mat4 GetNodeMatrix(tinygltf::Node* node);
        glm::vec3 GetPixelColor(float x, float y);
        void LoadNode(tinygltf::Model* model,
                      tinygltf::Node* node,
                      const glm::mat4& parentNodeMatrix,
                      glm::ivec2* imageSize);
        void LoadMeshes(tinygltf::Model* model, const std::string& modelPath);
        void LoadTexture(tinygltf::Model& model, int textureIndex,
                         const std::string& modelPath,
                         util::ImageData* imageData);
        void LoadNodes(tinygltf::Model* model, glm::ivec2* imageSize);
        void LoadIndices(tinygltf::Model& model,
                                          tinygltf::Primitive& primitive,
                                          const std::vector<Vertex>& vertices,
                                          std::vector<Triangle>* indices);
        void LoadVertices(tinygltf::Model& model,
                                         tinygltf::Primitive& primitive,
                                         std::vector<Vertex>* vertices,
                                         BoundingBox* bb);
        Material LoadMaterial(tinygltf::Model& model,
                              tinygltf::Primitive& primitive,
                              const std::string& modelPath);

    private:
        //resources to load
        std::map<int, Model> m_Models;
        std::vector<Transform> m_Transforms;
        std::map<int, Material> m_Materials;
        std::map<int, Texture> m_Textures;//all loaded images

        //to work then render
        std::vector<Camera> m_Cameras;
        std::vector<PointLight> m_PointLights;
        std::vector<Instance> m_Instances; // constain all instances of all meshes in the scene which should be rendered
        std::vector<Node> m_Scene;
    };
}