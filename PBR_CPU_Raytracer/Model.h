#pragma once

#include <vector>
#include <array>
#include <string>

#include "Ray.h"
#include "Globals.h"
#include "ImageHandler.h"
#include "BoundingBox.h"
#include "glm/glm.hpp"

constexpr float kEpsilon = 1e-8;

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
        glm::vec2 TexCoords0;
    };

    struct MeshPrimitive
    {
    public:
        std::vector<Vertex> Vertices;
        std::vector<Triangle> Triangles;
        BoundingBox Box; 
        uint32_t MaterialID;
    public:
        bool FindIntersection(Ray* localRay, Interval& interval, HitResult* result) const;
    private:
        bool IntersectsBox(Ray* localRay) const;
        bool RayIntersect(Ray* localRay, const Triangle* trianlge, Interval& interval, HitResult* result) const;
    };

    struct Model
    {
    public:
        std::string Name;
        std::vector<MeshPrimitive> Primitives;
        BoundingBox Box;
    public:
        bool FindIntersection(Ray* localRay, HitResult* result) const;
    private:
        bool IntersectsBox(Ray* localRay) const;
    };
}
