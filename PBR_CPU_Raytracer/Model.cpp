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

		for (auto& triangle : Triangles)
		{
			glm::vec3 me0 = Vertices[triangle.Indices[1]].Position - Vertices[triangle.Indices[0]].Position;
			glm::vec3 me1 = Vertices[triangle.Indices[2]].Position - Vertices[triangle.Indices[0]].Position;
			glm::vec3 barycentric;
			if (ray_triangle_intersect(localRay.Position, localRay.Direction,
				me0, me1, Vertices[triangle.Indices[0]].Position, localRay.Distance, barycentric))
			{
				if (localRay.Distance < ray->Distance)
				{
					glm::vec2 interpolatedTexCoords = (Vertices[triangle.Indices[0]].TexCoords * barycentric.x +
						Vertices[triangle.Indices[1]].TexCoords * barycentric.y +
						Vertices[triangle.Indices[2]].TexCoords * barycentric.z);

					glm::ivec2 texCoordsInPixel = { ImageData.Width * interpolatedTexCoords.x,
						ImageData.Height * interpolatedTexCoords.y };

					int pixelIndex = (texCoordsInPixel.y * ImageData.Width + texCoordsInPixel.x) * ImageData.NrChannels;
					ray->Color[0] = ImageData.Data[pixelIndex + 0];
					ray->Color[1] = ImageData.Data[pixelIndex + 1];
					ray->Color[2] = ImageData.Data[pixelIndex + 2];
					intersectionFound = true;
				}
			}
			//FindIntersectionInternal(&localRay, &triangle, &intersectionFound);
		}

		ray->Color = localRay.Color;
		return intersectionFound;
	}

	bool Mesh::FindIntersectionInternal(Ray* ray, const Triangle* triangle, bool* outIntersectionFound)
	{
		//if(math::ray_triangle_intersect(ray.m_origin, ray.m_direction, m_e0, m_e1, m_v0.m_position, dist, barycentric))


		float denom = glm::dot(triangle->Normal, ray->Direction);
		if (abs(denom) < 0.001f)
		{
			return false;
		}

		float distanceToPlane = -glm::dot(triangle->Normal, Vertices[triangle->Indices[0]].Position);
		float t = -(glm::dot(triangle->Normal, ray->Position) + distanceToPlane) / denom;

		if (t < 0)
		{
			return false;
		}

		glm::vec3 point = ray->Position + ray->Direction * t;

		//edge 0
		glm::vec3 edge = Vertices[triangle->Indices[1]].Position - Vertices[triangle->Indices[0]].Position;
		glm::vec3 vp = point - Vertices[triangle->Indices[0]].Position;
		glm::vec3 cross = glm::cross(edge, vp);

		if (glm::dot(triangle->Normal, cross) < 0)
		{
			return false;
		}

		//edge 1
		edge = Vertices[triangle->Indices[2]].Position - Vertices[triangle->Indices[1]].Position;
		vp = point - Vertices[triangle->Indices[1]].Position;
		cross = glm::cross(edge, vp);

		if (glm::dot(triangle->Normal, cross) < 0)
		{
			return false;
		}

		//edge 2
		edge = Vertices[triangle->Indices[0]].Position - Vertices[triangle->Indices[2]].Position;
		vp = point - Vertices[triangle->Indices[2]].Position;
		cross = glm::cross(edge, vp);

		if (glm::dot(triangle->Normal, cross) < 0)
		{
			return false;
		}

		if (t > ray->Distance)
		{
			return false;
		}

		//calculate barycentric coordinates
		//suppose we have triangle ABC and point of intersection P

		//calculate total triangle area
		glm::vec3 edge1 = Vertices[triangle->Indices[1]].Position - Vertices[triangle->Indices[0]].Position;
		glm::vec3 edge2 = Vertices[triangle->Indices[2]].Position - Vertices[triangle->Indices[0]].Position;
		float trArea = glm::length(glm::cross(edge1, edge2)) / 2.0f;

		//calculate u coeff
		glm::vec3 e1 = Vertices[triangle->Indices[0]].Position - point;
		glm::vec3 e2 = Vertices[triangle->Indices[1]].Position - point;
		float area = glm::length(glm::cross(e1, e2)) / 2.0f;
		float uCoeff = area / trArea;

		//calculate v coeff
		e1 = Vertices[triangle->Indices[1]].Position - point;
		e2 = Vertices[triangle->Indices[2]].Position - point;
		area = glm::length(glm::cross(e1, e2)) / 2.0f;
		float vCoeff = area / trArea;

		//calculate w coeff
		float wCoeff = 1.0f - uCoeff - vCoeff;

		//calculate interpolated texture cordinates
		glm::vec2 interpolatedTexCoords = (Vertices[triangle->Indices[0]].TexCoords * vCoeff +
			Vertices[triangle->Indices[1]].TexCoords * uCoeff +
			Vertices[triangle->Indices[2]].TexCoords * wCoeff);

		glm::ivec2 texCoordsInPixel = { ImageData.Width * interpolatedTexCoords.x,
			ImageData.Height * interpolatedTexCoords.y };

		int pixelIndex = (texCoordsInPixel.y * ImageData.Width + texCoordsInPixel.x) * ImageData.NrChannels;
		ray->Color[0] = ImageData.Data[pixelIndex + 0];
		ray->Color[1] = ImageData.Data[pixelIndex + 1];
		ray->Color[2] = ImageData.Data[pixelIndex + 2];

		ray->Distance = t;
		*outIntersectionFound = true;

		return true;
	}

	inline bool Mesh::ray_triangle_intersect(const glm::vec3& r0, 
		const glm::vec3& rd, 
		glm::vec3& v0, 
		const glm::vec3& v1, 
		const glm::vec3& v2, 
		float& distance, 
		glm::vec3& barycentric)
	{
		glm::vec3 e0 = v1 - v0;
		glm::vec3 e1 = v2 - v0;

		glm::vec3 pvec = cross(rd, e1);
		float det = glm::dot(e0, pvec);

		if (det < kEpsilon)
			return false;

		float inv_det = 1.0 / det;

		glm::vec3 tvec = r0 - v0;
		barycentric.y = dot(tvec, pvec) * inv_det;

		if (barycentric.y < 0.0f || barycentric.y > 1.0)
			return false;

		glm::vec3 qvec = cross(tvec, e0);
		barycentric.z = glm::dot(rd, qvec) * inv_det;

		if (barycentric.z < 0.0f || barycentric.y + barycentric.z > 1.0f)
			return false;

		barycentric.x = 1.0f - barycentric.y - barycentric.z;

		distance = glm::dot(e1, qvec) * inv_det;

		return true;
	}
}