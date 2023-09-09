#pragma once

#include <vector>
#include <array>

#include "glm/glm.hpp"

namespace pbr
{
    struct Triangle
    {
        std::array<unsigned int, 3> Indices;
    };

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    struct Mesh
    {
        std::string Name;
        std::vector<Vertex> Vertices;
        std::vector<Triangle> Triangles;
        glm::mat4 ModelMatrix;
        glm::vec4 BoundingBox; //empty for now
    };
}
