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

    glm::ivec2 filmSize = {0, pbr::HEIGHT};

    tinygltf::Model model;
    pbr::Scene scene;
    pbr::LoadGltf({ argv[4] }, &model);
    scene.Init(&model, &filmSize);
    pbr::Film film(filmSize.x, filmSize.y);
    film.ClearImage({ 0, 0, 0, 255 });

    scene.Render(&film);

    film.SaveCapture("Result.png");

    film.Free();

    return 0;
}