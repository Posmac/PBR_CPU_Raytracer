#include "ImageHandler.h"

namespace pbr
{
    namespace util
    {
        void ImageData::Init(int width, int height)
        {
            Width = width;
            Height = height;
            NrChannels = 4; // default 4 channels, 8 bits each
            Data = new uint8_t[Width * Height * NrChannels];
        }

        void ReadPng(const std::string& path, ImageData* imageData)
        {
            unsigned char* data = stbi_load(path.c_str(),
                                            &imageData->Width,
                                            &imageData->Height,
                                            &imageData->NrChannels,
                                            imageData->NrChannels);//4 channels required

            if(data)
            {
                imageData->Data = reinterpret_cast<uint8_t*>(data);
            }
        }

        void WriteToPng(std::string path, ImageData* imageData)
        {
            stbi_write_png(path.c_str(), 
                           imageData->Width, 
                           imageData->Height,
                           imageData->NrChannels, 
                           reinterpret_cast<unsigned char*>(imageData->Data),
                           imageData->Width * imageData->NrChannels);
        }

        void WriteColor(const glm::vec4& color, ImageData* imageData)
        {
            int size = imageData->Width * imageData->Height * imageData->NrChannels;
            for(int i = 0; i < size; i += imageData->NrChannels)
            {
                imageData->Data[i+0] = static_cast<uint8_t>(color.r);
                imageData->Data[i+1] = static_cast<uint8_t>(color.g);
                imageData->Data[i+2] = static_cast<uint8_t>(color.b);
                imageData->Data[i+3] = static_cast<uint8_t>(color.a);
            }
        }

        void SetPixelColor(const glm::ivec2& pixel,
                           const glm::vec4& color,
                           ImageData* imageData)
        {
            int pixelIndex = (pixel.y * imageData->Width + pixel.x) * imageData->NrChannels;
            imageData->Data[pixelIndex + 0] = color.r;
            imageData->Data[pixelIndex + 1] = color.g;
            imageData->Data[pixelIndex + 2] = color.b;
            imageData->Data[pixelIndex + 3] = color.a; 
        }

        void FreeMemory(ImageData* imageData)
        {
            stbi_image_free(imageData->Data);
        }
    }
}