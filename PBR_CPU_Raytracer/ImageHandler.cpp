#include "ImageHandler.h"

namespace pbr
{

    FileReadOpenData::FileReadOpenData(ImageData* data)
        : Data(data)
    {
    }

    FileWriteOpenData::FileWriteOpenData(ImageData* data)
        : Data(data)
    {
    }

    void ReadPng(const std::string& path, FileReadOpenData* imageData)
    {
        FILE* file = fopen(path.c_str(), "rb");
        
        png_structp str = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

        if(!str)
        {
            abort();
        }

        png_infop info = png_create_info_struct(str);

        if(!info)
        {
            abort();
        }

        if(setjmp(png_jmpbuf(str)))
        {
            abort();
        }

        png_init_io(str, file);
        png_read_info(str, info);

        imageData->Data->Width = png_get_image_width(str, info);
        imageData->Data->Height = png_get_image_height(str, info);
        imageData->Color_type = png_get_color_type(str, info);
        imageData->Bit_depth = png_get_bit_depth(str, info);

        // Read any color_type into 8bit depth, RGBA format.
        // See http://www.libpng.org/pub/png/libpng-manual.txt
        if(imageData->Bit_depth == 16)
        {
            png_set_strip_16(str);
        }

        if(imageData->Color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_palette_to_rgb(str);
        }

        // PNG_COLOR_TYPE_GRAY_ALPHA is always 8 or 16bit depth.
        if(imageData->Color_type == PNG_COLOR_TYPE_GRAY && imageData->Bit_depth < 8)
        {
            png_set_expand_gray_1_2_4_to_8(str);
        }

        if(png_get_valid(str, info, PNG_INFO_tRNS))
        {
            png_set_tRNS_to_alpha(str);
        }

        // These color_type don't have an alpha channel then fill it with 0xff.
        if(imageData->Color_type == PNG_COLOR_TYPE_RGB ||
           imageData->Color_type == PNG_COLOR_TYPE_GRAY ||
           imageData->Color_type == PNG_COLOR_TYPE_PALETTE)
        {
            png_set_filler(str, 0xFF, PNG_FILLER_AFTER);
        }

        if(imageData->Color_type == PNG_COLOR_TYPE_GRAY ||
           imageData->Color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        {
            png_set_gray_to_rgb(str);
        }

        png_read_update_info(str, info);

        if(!imageData->Data->Row_pointers)
        {
            abort();
        }

        imageData->Data->Row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * imageData->Data->Height);

        for(int y = 0; y < imageData->Data->Height; y++)
        {
            imageData->Data->Row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(str, info));
        }

        png_read_image(str, imageData->Data->Row_pointers);

        fclose(file);

        png_destroy_read_struct(&str, &info, NULL);
    }

    void WriteToPng(const std::string& path, FileWriteOpenData *imageData)
    {
        FILE* file = fopen(path.c_str(), "wb");

        if(!file)
        {
            abort();
        }

        png_structp str = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if(!str)
        {
            abort();
        }

        png_infop info = png_create_info_struct(str);
        if(!info)
        {
            abort();
        }

        if(setjmp(png_jmpbuf(str)))
        {
            abort();
        }

        png_init_io(str, file);

        // Output is 8bit depth, RGBA format.
        png_set_IHDR(
            str,
            info,
            imageData->Data->Width,
            imageData->Data->Height,
            8,
            PNG_COLOR_TYPE_RGBA,
            PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_DEFAULT,
            PNG_FILTER_TYPE_DEFAULT
        );
        png_write_info(str, info);

        // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
        // Use png_set_filler().
        //png_set_filler(png, 0, PNG_FILLER_AFTER);

        if(!imageData->Data->Row_pointers)
        {
            abort();
        }

        png_write_image(str, imageData->Data->Row_pointers);
        png_write_end(str, NULL);

        fclose(file);

        png_destroy_write_struct(&str, &info);
    }

    void WriteColor(glm::vec4& color, FileWriteOpenData* imageData)
    {
        for(int y = 0; y < imageData->Data->Height; y++)
        {
            png_bytep row = imageData->Data->Row_pointers[y];

            for(int x = 0; x < imageData->Data->Width; x++)
            {
                png_bytep px = &(row[x * 4]);
                px[0] = color.x;
                px[1] = color.y;
                px[2] = color.z;
                px[3] = color.a;

                // Do something awesome for each pixel here...
                //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
            }
        }
    }

    void FreeMemory(ImageData* imageData)
    {
        if(!imageData->Row_pointers)
        {
            __debugbreak();
            return;
        }

        for(int y = 0; y < imageData->Height; y++)
        {
            free(imageData->Row_pointers[y]);
        }
        free(imageData->Row_pointers);
    }

}