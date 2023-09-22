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
        bool FindIntersectionInternal(Ray* ray, const Triangle* triangle, bool* outIntersectionFound);

        inline bool ray_triangle_intersect(const glm::vec3& r0,
            const glm::vec3& rd,
            glm::vec3& v0,
            const glm::vec3& v1,
            const glm::vec3& v2,
            float& distance,
            glm::vec3& barycentric);
    };
}
