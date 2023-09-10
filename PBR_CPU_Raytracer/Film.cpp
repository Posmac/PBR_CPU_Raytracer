#include "Film.h"

#include <iostream>

namespace pbr
{
    Film::Film(int width, int height)
    {
        Width = width;
        Height = height;
        m_ImageData.Init(width, height);
    }

    void Film::SetPixelColor(const glm::ivec2& position, const glm::vec4& color)
    {
        util::SetPixelColor(position, color, &m_ImageData);
    }

    void Film::ClearImage(const glm::vec4 clearColor)
    {
        util::WriteColor(clearColor, &m_ImageData);
    }

    void Film::SaveCapture(const std::string& path)
    {
        util::WriteToPng(path, &m_ImageData);
    }

    void Film::Free()
    {
        util::FreeMemory(&m_ImageData);
    }
}