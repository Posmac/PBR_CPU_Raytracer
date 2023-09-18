#include "Model.h"
#include "Globals.h"

namespace pbr
{
    bool Mesh::FindIntersection(Ray* ray)
    {
        Ray localRay{ InvModelMatrix * glm::vec4(ray->Position, 1.0),
            glm::normalize(InvModelMatrix * glm::vec4(ray->Direction, 0.0)),
            ray->Distance };

        bool intersectionFound = false;

        for(auto& triangle : Triangles)
        {
            FindIntersectionInternal(&localRay, &triangle, &intersectionFound);
        }

        ray->Color = localRay.Color;
        return intersectionFound;
    }

    bool Mesh::FindIntersectionInternal(Ray* ray, const Triangle* triangle, bool* outIntersectionFound)
    {
        float denom = glm::dot(triangle->Normal, ray->Direction);
        if(abs(denom) < 0.001f)
        {
            return false;
        }

        float distanceToPlane = -glm::dot(triangle->Normal, Vertices[triangle->Indices[0]].Position);
        float t = -(glm::dot(triangle->Normal, ray->Position) + distanceToPlane) / denom;

        if(t < 0)
        {
            return false;
        }

        glm::vec3 point = ray->Position + ray->Direction * t;

        //edge 0
        glm::vec3 edge = Vertices[triangle->Indices[1]].Position - Vertices[triangle->Indices[0]].Position;
        glm::vec3 vp = point - Vertices[triangle->Indices[0]].Position;
        glm::vec3 cross = glm::cross(edge, vp);

        if(glm::dot(triangle->Normal, cross) < 0)
        {
            return false;
        }

        //edge 1
        edge = Vertices[triangle->Indices[2]].Position - Vertices[triangle->Indices[1]].Position;
        vp = point - Vertices[triangle->Indices[1]].Position;
        cross = glm::cross(edge, vp);

        if(glm::dot(triangle->Normal, cross) < 0)
        {
            return false;
        }

        //edge 2
        edge = Vertices[triangle->Indices[0]].Position - Vertices[triangle->Indices[2]].Position;
        vp = point - Vertices[triangle->Indices[2]].Position;
        cross = glm::cross(edge, vp);

        if(glm::dot(triangle->Normal, cross) < 0)
        {
            return false;
        }

        if(t > ray->Distance)
        {
            return false;
        }

        ray->Distance = t;
        *outIntersectionFound = true;
        ray->Color = triangle->Color;;

        return true;
    }
}