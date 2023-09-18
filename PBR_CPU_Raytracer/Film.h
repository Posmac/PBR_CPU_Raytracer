#pragma once

#include "ImageHandler.h"

namespace pbr
{
    class Film
    {
    public:
        Film(int width, int height);
        void SetPixelColor(const glm::ivec2& position, const glm::vec3& color);
        void ClearImage(const glm::vec4 clearColor);
        void SaveCapture(const std::string& path);
        void Free();
    public:
        int Width;
        int Height;
        util::ImageData ImageData;
    };
}