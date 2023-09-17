#include "ImageHandler.h"
#include <algorithm>

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
                           const glm::vec4& color,
                           ImageData* imageData)
        {
            int pixelIndex = (pixel.y * imageData->Width + pixel.x) * imageData->NrChannels;
            glm::ivec4 ncolor = color * 255.0f;
            ncolor = glm::clamp(ncolor, glm::ivec4(0), glm::ivec4(255));

            //if(pixelIndex+0>(imageData->Width* imageData->Height* imageData->NrChannels)) __debugbreak();
            imageData->Data[pixelIndex + 0] = static_cast<unsigned char>(ncolor.r);
            //if(pixelIndex + 1>(imageData->Width * imageData->Height * imageData->NrChannels)) __debugbreak();
            imageData->Data[pixelIndex + 1] = static_cast<unsigned char>(ncolor.g);
            //if(pixelIndex + 2 > (imageData->Width * imageData->Height * imageData->NrChannels)) __debugbreak();
            imageData->Data[pixelIndex + 2] = static_cast<unsigned char>(ncolor.b);
            //if(pixelIndex + 3 > (imageData->Width * imageData->Height * imageData->NrChannels)) __debugbreak();
            imageData->Data[pixelIndex + 3] = static_cast<unsigned char>(ncolor.a);
        }

        void FreeMemory(ImageData* imageData)
        {
            stbi_image_free(imageData->Data);
        }
    }
}