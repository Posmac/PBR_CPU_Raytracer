#pragma once

#include "glm/glm.hpp"

namespace pbr
{
    struct Interval
    {
        inline Interval()
        {
            Reset();
        }

        inline Interval(float min, float max)
            : Min(min), Max(max)
        {
        }

        inline void SetNewInterval(float min, float max)
        {
            Min = min;
            Max = max;
        }

        inline void Reset()
        {
            Min = -std::numeric_limits<float>::max();
            Max = std::numeric_limits<float>::max();
        }

        inline bool IsInsideInterval(float t)
        {
            return t > Min && t < Max;
        }

        float Min;
        float Max;
    };

    struct Ray
    {
        inline Ray()
        {
            
        }

        inline Ray(const glm::vec4& pos, const glm::vec4& dir)
            : Position(pos), Direction(dir)
        {
        }

        glm::vec3 Position;
        glm::vec3 Direction;
    };

    inline Ray TransformRay(const Ray& ray, const glm::mat4& matrix)
    {
        return { matrix * glm::vec4(ray.Position, 1.0), glm::normalize(matrix * glm::vec4(ray.Direction, 0.0)) };
    }

    struct HitResult
    {
        glm::vec3 LocalSpaceIntersectinPoint;
        glm::vec3 LocalSpaceNormal;
        bool FrontFace;
        glm::vec3 BarycentricCoordinates;

        glm::vec3 WorldSpaceIntersectinPoint;
        glm::vec3 WorldSpaceNormal;
        size_t PrimitiveID;
        size_t MaterialID;
    };
}