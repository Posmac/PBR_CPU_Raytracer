#pragma once

#include <vector>
#include <array>
#include <string>

#include "Ray.h"
#include "Globals.h"
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
        glm::vec3 Color;
    };

    struct Mesh
    {
        std::string Name;
        std::vector<Vertex> Vertices;
        std::vector<Triangle> Triangles;
        glm::mat4 ModelMatrix;
        glm::mat4 InvModelMatrix;
        glm::vec4 BoundingBox; //empty for now

        bool FindIntersection(Ray* ray);
        bool FindIntersectionInternal(Ray* ray, const Triangle* triangle, bool* outIntersectionFound);
    };
}
