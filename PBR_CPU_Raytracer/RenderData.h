#pragma once

#include "glm/glm.hpp"
#include "ImageHandler.h"
#include "KhrExtensions.h"

namespace pbr
{
    enum AlphaMode
    {
        OPAQ,
    };

    struct Sampler
    {
        //for now empty
    };

    struct Texture
    {
    public:
        util::ImageData ImageData;
        int TexCoordsIndex;
    public:
        inline glm::vec4 Sample(glm::ivec2 texCoords)
        {
            if(ImageData.Data == nullptr)
            {
                return { 0,0,0,1 };
            }

            glm::ivec2 texCoordsInPixel = { ImageData.Width * texCoords.x, ImageData.Height * texCoords.y };
            texCoordsInPixel.x %= ImageData.Width; //repeating sampling
            texCoordsInPixel.y %= ImageData.Height; //repeating sampling

            int pixelIndex = (texCoordsInPixel.y * ImageData.Width + texCoordsInPixel.x) * ImageData.NrChannels;

            return
            {
                ImageData.Data[pixelIndex + 0],
                ImageData.Data[pixelIndex + 1],
                ImageData.Data[pixelIndex + 2],
                ImageData.Data[pixelIndex + 3]
            };
        }
    };

    struct Material
    {
        Texture* Normal = nullptr;
        Texture* Occlusion = nullptr;
        Texture* BaseColor = nullptr;
        Texture* MetallicRoughness = nullptr;
        Texture* Emissive = nullptr;

        glm::vec3 emissiveFactor;
        AlphaMode alphaMode; 
        float alphaCutoff;
        bool doubleSided;
        glm::vec4 baseColorFactor;
        float metallicFactor;
        float roughtnessFactor;
        float normalTextureScale;
        float occlusionTextureStrenght;

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
        uint32_t modelID;
        uint32_t transformID;
        std::vector<uint32_t> primitiveMaterialsID;
    };
}