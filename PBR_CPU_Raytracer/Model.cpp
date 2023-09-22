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
		glm::vec3 barycentric;

		for (auto& triangle : Triangles)
		{
			if (RayIntersect(&localRay, &triangle, barycentric))
			{
				if (localRay.Distance < ray->Distance)
				{
					glm::vec2 interpolatedTexCoords = (Vertices[triangle.Indices[0]].TexCoords * barycentric.x +
						Vertices[triangle.Indices[1]].TexCoords * barycentric.y +
						Vertices[triangle.Indices[2]].TexCoords * (1.0f - barycentric.x - barycentric.y));

					glm::ivec2 texCoordsInPixel = { ImageData.Width * interpolatedTexCoords.x,
						ImageData.Height * interpolatedTexCoords.y };

					int pixelIndex = (texCoordsInPixel.y * ImageData.Width + texCoordsInPixel.x) * ImageData.NrChannels;
					localRay.Color[0] = ImageData.Data[pixelIndex + 0];
					localRay.Color[1] = ImageData.Data[pixelIndex + 1];
					localRay.Color[2] = ImageData.Data[pixelIndex + 2];
					//localRay.Color = { barycentric.x, barycentric.y, (1.0f - barycentric.x - barycentric.y) };
					ray->Distance = localRay.Distance;
					intersectionFound = true;
				}
			}
		}

		ray->Color = localRay.Color;
		return intersectionFound;
	}

	bool Mesh::RayIntersect(Ray* ray, 
		Triangle* trianlge,
		glm::vec3& barycentric)
	{
		glm::vec3 v0v1 = Vertices[trianlge->Indices[1]].Position - Vertices[trianlge->Indices[0]].Position;
		glm::vec3 v0v2 = Vertices[trianlge->Indices[2]].Position - Vertices[trianlge->Indices[0]].Position;

		glm::vec3 N = glm::cross(v0v1, v0v2);
		float denom = glm::dot(N, N);

		float NdotRayDirection = glm::dot(N, ray->Direction);
		if (fabs(NdotRayDirection) < kEpsilon)
			return false;

		float d = -glm::dot(N, Vertices[trianlge->Indices[0]].Position);

		float t = -(glm::dot(N, ray->Position) + d) / NdotRayDirection;
		if (t < 0) 
			return false; 

		glm::vec3 P = ray->Position + t * ray->Direction;
		glm::vec3 C;

		// edge 0
		glm::vec3 edge0 = Vertices[trianlge->Indices[1]].Position - Vertices[trianlge->Indices[0]].Position;
		glm::vec3 vp0 = P - Vertices[trianlge->Indices[0]].Position;
		C = glm::cross(edge0, vp0);
		if (glm::dot(N, C) < 0) 
			return false;

		// edge 1
		glm::vec3 edge1 = Vertices[trianlge->Indices[2]].Position - Vertices[trianlge->Indices[1]].Position;
		glm::vec3 vp1 = P - Vertices[trianlge->Indices[1]].Position;
		C = glm::cross(edge1, vp1);
		if ((barycentric.x = glm::dot(N, C)) < 0)  
			return false; 

		// edge 2
		glm::vec3 edge2 = Vertices[trianlge->Indices[0]].Position - Vertices[trianlge->Indices[2]].Position;
		glm::vec3 vp2 = P - Vertices[trianlge->Indices[2]].Position;
		C = glm::cross(edge2, vp2);
		if ((barycentric.y = glm::dot(N, C)) < 0) 
			return false;

		barycentric /= denom;

		ray->Distance = t;

		return true;
	}
}