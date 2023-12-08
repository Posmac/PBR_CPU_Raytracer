#include "Model.h"
#include "Globals.h"


namespace pbr
{
    bool MeshPrimitive::FindIntersection(Ray* localRay, Interval& interval, HitResult* result) const
    {
        /*if(!IntersectsBox(localRay))
        {
            return false;
        }*/

        bool intersectionFound = false;

        for(size_t i = 0; i < Triangles.size(); i++)
        {
            if(RayIntersect(localRay, &Triangles[i], interval, result))
            {
                intersectionFound = true;
            }
        }

        return intersectionFound;
    }

    bool MeshPrimitive::IntersectsBox(Ray* localRay) const
    {
        return Box.FindIntersection(*localRay);
    }

    bool MeshPrimitive::RayIntersect(Ray* ray, const Triangle* trianlge, Interval& interval, HitResult* result) const
    {
        glm::vec3 v0v1 = Vertices[trianlge->Indices[1]].Position - Vertices[trianlge->Indices[0]].Position;
        glm::vec3 v0v2 = Vertices[trianlge->Indices[2]].Position - Vertices[trianlge->Indices[0]].Position;

        glm::vec3 N = glm::cross(v0v1, v0v2);
        float denom = glm::dot(N, N);

        float NdotRayDirection = glm::dot(N, ray->Direction);
        if(fabs(NdotRayDirection) < kEpsilon)
        {
            return false;
        }

        float d = -glm::dot(N, Vertices[trianlge->Indices[0]].Position);

        float t = -(glm::dot(N, ray->Position) + d) / NdotRayDirection;
        if(t < 0)
        {
            return false;
        }

        if(t > interval.Max)
        {
            return false;
        }

        glm::vec3 P = ray->Position + t * ray->Direction;

        // edge 0
        glm::vec3 edge0 = Vertices[trianlge->Indices[1]].Position - Vertices[trianlge->Indices[0]].Position;
        glm::vec3 vp0 = P - Vertices[trianlge->Indices[0]].Position;
        glm::vec3 C1 = glm::cross(edge0, vp0);

        if(glm::dot(N, C1) < 0)
        {
            return false;
        }

        // edge 1
        glm::vec3 edge1 = Vertices[trianlge->Indices[2]].Position - Vertices[trianlge->Indices[1]].Position;
        glm::vec3 vp1 = P - Vertices[trianlge->Indices[1]].Position;
        glm::vec3 C2 = glm::cross(edge1, vp1);
        if(glm::dot(N, C2) < 0)
        {
            return false;
        }

        // edge 2
        glm::vec3 edge2 = Vertices[trianlge->Indices[0]].Position - Vertices[trianlge->Indices[2]].Position;
        glm::vec3 vp2 = P - Vertices[trianlge->Indices[2]].Position;
        glm::vec3 C3 = glm::cross(edge2, vp2);

        if(glm::dot(N, C3) < 0)
        {
            return false;
        }

        result->BarycentricCoordinates.x = glm::dot(N, C2);
        result->BarycentricCoordinates.y = glm::dot(N, C3);
        result->BarycentricCoordinates /= denom;
        result->LocalSpaceNormal = glm::normalize(N);
        result->LocalSpaceIntersectinPoint = P;
        interval.Max = t;

        return true;
    }

    bool Model::FindIntersection(Ray* localRay, HitResult* result) const
    {
        if(!IntersectsBox(localRay))
        {
            return false;
        }

        bool intersectionFind = false;
        Interval interval = { 0, std::numeric_limits<float>::infinity() };

        for(size_t i = 0; i < Primitives.size(); i++)
        {
            if(Primitives[i].FindIntersection(localRay, interval, result))
            {
                intersectionFind = true;
                result->PrimitiveID = i;
            }
        }

        return intersectionFind;
    }

    bool Model::IntersectsBox(Ray* localRay) const
    {
        return Box.FindIntersection(*localRay);
    }
}