#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "Core.h"
#include "stb_image.h"
#include "stb_image_write.h"

#include "glm/glm.hpp"

namespace pbr
{
    namespace util
    {
        struct ImageData
        {
            int Width;
            int Height;
            int NrChannels;
            int Size;
            unsigned char* Data = nullptr;

            void Init(int width, int height);
        };

        void ReadPng(const std::string& path, ImageData* imageData);
        void WriteToPng(std::string path, ImageData* imageData);
        void WriteColor(const glm::vec4& color, ImageData* imageData);
        void SetPixelColor(const glm::ivec2& pixel,
                           glm::vec3& color,
                           ImageData* imageData);
        void FreeMemory(ImageData* imageData);
    }
}