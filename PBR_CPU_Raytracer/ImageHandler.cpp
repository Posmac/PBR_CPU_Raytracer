#include "ImageHandler.h"
#include <cmath>
#include <algorithm>

#include "Globals.h"

namespace pbr
{
    namespace util
    {
        void ImageData::Init(int width, int height)
        {
            Width = width;
            Height = height;
            NrChannels = 4; // default 4 channels, 8 bits each
            Data = new unsigned char[Width * Height * NrChannels];
        }

        void ReadPng(const std::string& path, ImageData* imageData)
        {
            imageData->Data = stbi_load(path.c_str(),
                                        &imageData->Width,
                                        &imageData->Height,
                                        &imageData->NrChannels,
                                        4);//4 channels required
        }

        void WriteToPng(std::string path, ImageData* imageData)
        {
            stbi_write_png(path.c_str(), 
                           imageData->Width, 
                           imageData->Height,
                           imageData->NrChannels, 
                           imageData->Data,
                           imageData->Width * imageData->NrChannels);
        }

        void WriteColor(const glm::vec4& color, ImageData* imageData)
        {
            int size = imageData->Width * imageData->Height * imageData->NrChannels;
            for(int i = 0; i < size; i += imageData->NrChannels)
            {
                imageData->Data[i+0] = static_cast<unsigned char>(color.r);
                imageData->Data[i+1] = static_cast<unsigned char>(color.g);
                imageData->Data[i+2] = static_cast<unsigned char>(color.b);
                imageData->Data[i+3] = static_cast<unsigned char>(color.a);
            }
        }

        void SetPixelColor(const glm::ivec2& pixel,
                           glm::vec3& color,
                           ImageData* imageData)
        {
            color = glm::pow(color, glm::vec3(1.0f/2.2f)); //gamma correction
            color /= (color + 1.0f); //Reinhard tone-mapping
            color = glm::clamp(color * 255.99f, glm::vec3(0.0f), glm::vec3(255.0f));

            int pixelIndex = (pixel.y * imageData->Width + pixel.x) * imageData->NrChannels;

            imageData->Data[pixelIndex + 0] = color.r;
            imageData->Data[pixelIndex + 1] = color.g;
            imageData->Data[pixelIndex + 2] = color.b;
            imageData->Data[pixelIndex + 3] = 255;
        }

        void FreeMemory(ImageData* imageData)
        {
            stbi_image_free(imageData->Data);
        }
    }
}