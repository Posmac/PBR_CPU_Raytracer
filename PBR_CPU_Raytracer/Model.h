#pragma once

#include <vector>
#include <array>
#include <string>

#include "Ray.h"
#include "Globals.h"
#include "ImageHandler.h"
#include "glm/glm.hpp"

constexpr float kEpsilon = 1e-8;

namespace pbr
{
    struct Triangle
    {
        std::array<unsigned int, 3> Indices;
        glm::vec3 Normal;
        glm::vec3 Color;
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
        util::ImageData ImageData;
        glm::vec4 BoundingBox; //empty for now

        bool FindIntersection(Ray* ray);
        bool RayIntersect(Ray* ray, Triangle* trianlge, glm::vec3& barycentric);
    };
}
