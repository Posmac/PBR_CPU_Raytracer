#include "Film.h"

#include <iostream>

namespace pbr
{
    Film::Film(int width, int height)
    {
        m_Width = width;
        m_Height = height;
        m_ImageData.Init(width, height);
    }

    void Film::SetPixelColor(const glm::vec2 position, const glm::vec4& color)
    {
        int x = static_cast<int>(m_Width * position.x);
        int y = static_cast<int>(m_Height * position.y);
        util::SetPixelColor({x,y}, color, &m_ImageData);
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