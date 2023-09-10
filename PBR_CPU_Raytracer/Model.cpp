#include "Model.h"

#include <iostream>

namespace pbr
{
    bool Mesh::FindIntersection(Ray* ray)
    {
        glm::vec3 localRayDir = InvModelMatrix * glm::vec4(ray->Direction, 1.0);
        glm::vec3 localRayPos = InvModelMatrix * glm::vec4(ray->Position, 1.0);
        bool intersectionFound = false;

        Ray localRay;
        localRay.Direction = localRayDir;
        localRay.Position = localRayPos;
        localRay.Distance = ray->Distance;

        for(auto& triangle : Triangles)
        {
            intersectionFound = FindIntersectionInternal(&localRay, &triangle);
        }

        return intersectionFound;
    }

    bool Mesh::FindIntersectionInternal(Ray* ray, const Triangle* triangle)
    {
        glm::vec3 triangleNormal = (Vertices[triangle->Indices[0]].Normal +
                                    Vertices[triangle->Indices[1]].Normal +
                                    Vertices[triangle->Indices[2]].Normal) / 3.0f;

        float denom = glm::dot(triangleNormal, ray->Direction);
        if(abs(denom) < 0.001f)
        {
            return false;
        }

        float distanceToPlane = -glm::dot(triangleNormal, Vertices[triangle->Indices[0]].Position);
        float t = -(glm::dot(triangleNormal, ray->Position) + distanceToPlane) / denom;

        if(t < 0)
        {
            return false;
        }

        glm::vec3 point = ray->Position + ray->Direction * t;

        //edge 0
        glm::vec3 edge = Vertices[triangle->Indices[1]].Position - Vertices[triangle->Indices[0]].Position;
        glm::vec3 vp = point - Vertices[triangle->Indices[0]].Position;
        glm::vec3 cross = glm::cross(edge, vp);

        if(glm::dot(triangleNormal, cross) < 0)
        {
            return false;
        }

        //edge 1
        edge = Vertices[triangle->Indices[2]].Position - Vertices[triangle->Indices[1]].Position;
        vp = point - Vertices[triangle->Indices[1]].Position;
        cross = glm::cross(edge, vp);

        if(glm::dot(triangleNormal, cross) < 0)
        {
            return false;
        }

        //edge 2
        edge = Vertices[triangle->Indices[0]].Position - Vertices[triangle->Indices[2]].Position;
        vp = point - Vertices[triangle->Indices[2]].Position;
        cross = glm::cross(edge, vp);

        if(glm::dot(triangleNormal, cross) < 0)
        {
            return false;
        }

        if(t > ray->Distance)
        {
            return false;
        }

        ray->Distance = t;

        return true;
    }
}