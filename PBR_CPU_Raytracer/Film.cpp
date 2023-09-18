#include "Film.h"

#include <iostream>

namespace pbr
{
    Film::Film(int width, int height)
    {
        Width = width;
        Height = height;
        ImageData.Init(width, height);
    }

    void Film::SetPixelColor(const glm::ivec2& position, const glm::vec3& color)
    {
        util::SetPixelColor(position, color, &ImageData);
    }

    void Film::ClearImage(const glm::vec4 clearColor)
    {
        util::WriteColor(clearColor, &ImageData);
    }

    void Film::SaveCapture(const std::string& path)
    {
        util::WriteToPng(path, &ImageData);
    }

    void Film::Free()
    {
        util::FreeMemory(&ImageData);
    }
}