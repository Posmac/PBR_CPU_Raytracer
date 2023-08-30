#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "zlib/zlib.h"
#include "libpng/png.h"

#include "glm/glm.hpp"

namespace pbr
{
    struct ImageData
    {
        int Width;
        int Height;
        png_bytep* Row_pointers;
        
    };

    struct FileReadOpenData
    {
        explicit FileReadOpenData(ImageData* data);
        png_byte Color_type;
        png_byte Bit_depth;
        ImageData* Data;
    };

    struct FileWriteOpenData
    {
        explicit FileWriteOpenData(ImageData* data);
        ImageData* Data;
    };

    void ReadPng(const std::string& path, FileReadOpenData* imageData);
    void WriteToPng(const std::string& path, FileWriteOpenData* imageData);
    void WriteColor(glm::vec4& color, FileWriteOpenData* imageData);
    void FreeMemory(ImageData* imageData);
}