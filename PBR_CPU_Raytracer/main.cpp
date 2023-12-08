#include <iostream>
#include <string>
#include <Windows.h>
#include <thread>

#include "GltfLoader.h"
#include "Scene.h"
#include "Film.h"
#include "Globals.h"

#include "tinygltf/tiny_gltf.h"

#include "glm/glm.hpp"

#include <easy/profiler.h>

int main(int argc, char* argv[])
{
    EASY_PROFILER_ENABLE;
    EASY_MAIN_THREAD;
    profiler::startListen();

    std::string defaultModelPath = "Duck.glb";
    std::string defaultSavePath = "Result.png";
    glm::ivec2 filmSize = { pbr::WIDTH, pbr::HEIGHT };

    for(int i = 0; i < argc; i++)
    {
        if(strcmp(argv[i], "--in") == 0)
        {
            defaultModelPath = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "--out") == 0)
        {
            defaultSavePath = argv[i + 1];
            i++;
        }
        else if(strcmp(argv[i], "--height") == 0)
        {
            //filmSize.y = std::atoi(argv[i + 1]);
            i++;
        }
    }

    pbr::LogInfo("Started process of parsing");

    EASY_BLOCK("Model");
    tinygltf::Model model;
    pbr::Scene scene;
    pbr::LoadGltf(defaultModelPath, &model);
    pbr::LogInfo("Loaded gltf");
    EASY_END_BLOCK;

    EASY_BLOCK("Scene");
    scene.Init(&model, &filmSize, defaultModelPath);
    pbr::Film film(filmSize.x, filmSize.y);
    pbr::LogInfo("Initialized scene");
    EASY_END_BLOCK;

    EASY_BLOCK("Image");
    film.ClearImage({ 0, 0, 0, 255 });
    pbr::LogInfo("Prepared image: " + std::to_string(film.Width) + " " + std::to_string(film.Height));
    EASY_END_BLOCK;

    pbr::LogInfo("Start rendering");
    scene.Render(&film);
    pbr::LogInfo("End rendering");

    film.SaveCapture(defaultSavePath);
    pbr::LogInfo("Saved to image");

    film.Free();
    profiler::stopListen();
    return 0;
}