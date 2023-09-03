#include "GltfLoader.h"

#define TINYGLTF_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"

namespace pbr
{
    void LoadGltf(const std::string& path, tinygltf::Model* model)
    {
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = false;
        if(path.find(".gltf") != std::string::npos)
        {
            ret = loader.LoadASCIIFromFile(model, &err, &warn, path);
        }
        else if(path.find(".glb") != std::string::npos)
        {
            ret = loader.LoadBinaryFromFile(model, &err, &warn, path); // for binary glTF(.glb)
        }

        if(!warn.empty())
        {
            printf("Warn: %s\n", warn.c_str());
        }

        if(!err.empty())
        {
            printf("Err: %s\n", err.c_str());
        }

        AssertIfTrue(!ret);
    }
}