#pragma once

#include <vector>
#include <algorithm>
#include <unordered_set>
#include <fstream>

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

#include "KhrExtensions.h"

namespace pbr
{
    struct Sampler
    {
        //for now empty
    };

    struct Texture
    {
        util::ImageData* ImageData;
        inline glm::vec4 Sample(glm::ivec2 texCoords)
        {
            if(ImageData == nullptr)
            {
                return { 0,0,0,1 };
            }

            glm::ivec2 texCoordsInPixel = { ImageData->Width * texCoords.x, ImageData->Height * texCoords.y };
            texCoordsInPixel.x %= ImageData->Width;
            texCoordsInPixel.y %= ImageData->Height;

            int pixelIndex = (texCoordsInPixel.y * ImageData->Width + texCoordsInPixel.x) * ImageData->NrChannels;

            return 
            {
                ImageData->Data[pixelIndex + 0],
                ImageData->Data[pixelIndex + 1],
                ImageData->Data[pixelIndex + 2],
                ImageData->Data[pixelIndex + 3]
            };
        }
    };

    struct Material
    {
        Texture* Normal;
        Texture* OcclusionTexture;
        Texture* BaseColorTexture;
        Texture* MetallicRoughnessTexture;

        KHR_materials_transmission Transmission;
        KHR_materials_volume Volume;
    };

    struct Transform
    {
        glm::mat4 ModelMatrix;
        glm::mat4 InvModelMatrix;
    };

    struct Instance
    {
        uint32_t meshId;
        uint32_t transformId;
        uint32_t materialId;
    };

    class Scene
    {
    public:
        Scene();
        void Init(tinygltf::Model* model, glm::ivec2* imageSize);
        void Render(Film* film);
        glm::mat4 GetNodeMatrix(tinygltf::Node* node);
        glm::vec3 GetPixelColor(float x, float y);
        void LoadNode(tinygltf::Model* model, tinygltf::Node* node, const glm::mat4& parentNodeMatrix, glm::ivec2* imageSize);
        void LoadMeshes();
        void LoadTextures();
    private:
        //resources to load
        std::vector<Mesh> m_Meshes;
        std::vector<Transform> m_Transforms;
        std::vector<Material> m_Material;
        std::vector<Texture> m_Textures;
        std::vector<util::ImageData> m_Images; //all loaded images

        //to work then render
        std::vector<Camera> m_Cameras;
        std::vector<PointLight> m_PointLights;
        std::vector<Instance> m_Instances; // constain all instances of all meshes in the scene which should be rendered
    };
}