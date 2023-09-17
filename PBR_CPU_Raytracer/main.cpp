#include <iostream>
#include <string>

#include "GltfLoader.h"
#include "Scene.h"
#include "Film.h"
#include "Globals.h"

#include "tinygltf/tiny_gltf.h"

#include "glm/glm.hpp"

int main(int argc, char* argv[])
{
    if(argc != 5)
    {
        std::cout << "Insufficient number of params" << std::endl;
        abort();
    }

    pbr::LogInfo("Started process of parsing");

    glm::ivec2 filmSize = { pbr::WIDTH, pbr::HEIGHT};

    tinygltf::Model model;
    pbr::Scene scene;
    pbr::LoadGltf({ argv[4] }, &model);
    pbr::LogInfo("Loaded gltf");

    scene.Init(&model, &filmSize);
    pbr::Film film(filmSize.x, filmSize.y);
    pbr::LogInfo("Initialized scene");

    film.ClearImage({ 0, 0, 0, 255 });
    pbr::LogInfo("Prepared image");

    pbr::LogInfo("Start rendering");
    scene.Render(&film);
    pbr::LogInfo("End rendering");

    film.SaveCapture("Result.png");
    pbr::LogInfo("Saved to image");

    film.Free();

    return 0;
}