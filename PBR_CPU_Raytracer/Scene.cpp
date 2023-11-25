#include "Scene.h"
#include <thread>
#include <string>

#include <easy/profiler.h>

namespace pbr
{
	Scene::Scene()
	{

	}

	void Scene::Init(tinygltf::Model* model, glm::ivec2* imageSize,
		const std::string& modelPath)
	{
		std::string path = modelPath;
		size_t offset = path.find_last_of('\\');
		path.resize(offset + 1);

		LoadMeshes(model, path);
		LoadNodes(model, imageSize);

        if(m_Cameras.empty())
        {
            LogInfo("Gltf file dont contain any camera, created the basic one");
            glm::mat4 view = glm::lookAtRH(glm::vec3(-300, -500, 1000), glm::vec3(0), glm::vec3(0, 1, 0));
            imageSize->x = WIDTH;
            imageSize->y = HEIGHT;
            m_Cameras.emplace_back(Camera(glm::radians(60.0f), 0.1f, 10000.0f, *imageSize, view));
        }
    }

	void Scene::LoadNode(tinygltf::Model* model,
		tinygltf::Node* node, const glm::mat4& parentNodeMatrix,
		glm::ivec2* imageSize)
	{
		glm::mat4 nodeMatrix = GetNodeMatrix(node);
		if (node->camera != -1)
		{
			tinygltf::Camera* camera = &model->cameras[node->camera];
			if (camera->type == "perspective")
			{
				float yFov = camera->perspective.yfov;
				float aspectRatio = camera->perspective.aspectRatio;
				float znear = camera->perspective.znear;
				float zfar = camera->perspective.zfar;

                glm::mat4 view = parentNodeMatrix * nodeMatrix;
                imageSize->x = imageSize->y * aspectRatio;
                m_Cameras.emplace_back(Camera(yFov, znear, zfar, *imageSize, glm::inverse(view)));
            }
        }
        else if(node->light != -1)
        {
            PointLight light{};
            light.WorldMatrix = nodeMatrix;
            m_PointLights.emplace_back(light);
        }
        else if(node->mesh != -1)
        {
            tinygltf::Mesh curMesh = model->meshes[node->mesh];

            Transform transform;
            transform.ModelMatrix = nodeMatrix;
            transform.InvModelMatrix = glm::inverse(transform.ModelMatrix);

            Instance instance;
            instance.modelID = node->mesh;
            instance.transformID = m_Transforms.size();

            for(auto& primitive : curMesh.primitives)
            {
                if(primitive.material == -1)
                {
                    LogInfo("No material found!");
                    continue;
                }
                instance.primitiveMaterialsID.push_back(primitive.material);
            }

            m_Transforms.push_back(std::move(transform));
            m_Instances.push_back(instance);
        }
    }

	void Scene::LoadMeshes(tinygltf::Model* curModel, const std::string& modelPath)
	{
		for (int meshId = 0; meshId < curModel->meshes.size(); meshId++)
		{
			if (m_Models.count(meshId) != 0)
			{
				LogInfo("Found the same mesh model");
				__debugbreak();
				continue;
			}

			tinygltf::Mesh* curMesh = &curModel->meshes[meshId];
			Model model;
			model.Name = curMesh->name;
			model.Box.Reset();

			for (auto& primitive : curMesh->primitives)
			{
				//render mode not used for now
				int mode = primitive.mode;
				MeshPrimitive meshPrimitive;

				LoadVertices(*curModel, primitive, &meshPrimitive.Vertices, &meshPrimitive.Box);
				LoadIndices(*curModel, primitive, meshPrimitive.Vertices, &meshPrimitive.Triangles);

				model.Box.Max = glm::max(model.Box.Max, meshPrimitive.Box.Max);
				model.Box.Min = glm::min(model.Box.Min, meshPrimitive.Box.Min);

                Material material = LoadMaterial(*curModel, primitive, modelPath);

                meshPrimitive.MaterialID = primitive.material;

                if(primitive.material != -1 && m_Materials.count(primitive.material) == 0)
                {
                    m_Materials[primitive.material] = std::move(material);
                }

				model.Primitives.push_back(std::move(meshPrimitive));
			}

			/*mesh.ModelMatrix = parentNodeMatrix * GetNodeMatrix(node);
			mesh.InvModelMatrix = glm::inverse(mesh.ModelMatrix);*/
			if (m_Models.count(meshId) == 0)
			{
				m_Models[meshId] = std::move(model);
			}
		}

		LogInfo("Total models: " + std::to_string(m_Models.size()));
		LogInfo("Total materials: " + std::to_string(m_Materials.size()));
	}

	void Scene::LoadTexture(tinygltf::Model& model, int textureIndex, const std::string& modelPath, util::ImageData* imageData)
	{
		tinygltf::Texture curTexture = model.textures[textureIndex];

		if (curTexture.source != -1)
		{
			tinygltf::Image curImage = model.images[curTexture.source];
			if (!curImage.uri.empty())
			{
				stbi_uc* data = nullptr;

				if (curImage.mimeType == "image/png")
				{
					data = stbi_load((modelPath + curImage.uri).c_str(), &imageData->Width, &imageData->Height, &imageData->NrChannels, curImage.component);
				}
				else
				{
					data = stbi_load((modelPath + curImage.uri).c_str(), &imageData->Width, &imageData->Height, &imageData->NrChannels, 0);
				}

				if (data)
				{
					imageData->Data = data;
					imageData->Size = curImage.image.size();
				}
				else
				{
					__debugbreak();
				}
			}
			else if (curImage.bufferView != -1)
			{
				switch (curImage.bits)
				{
				case 8:
				{
					auto& bufferView = model.bufferViews[curImage.bufferView];
					auto& buffer = model.buffers[bufferView.buffer];

					if (curImage.component == 3)
					{
						/*int bufferSize = curImage.width * curImage.height * 4;
						unsigned char* buffer = new unsigned char[bufferSize];
						unsigned char* rgba = buffer;
						unsigned char* rgb = &curImage.image[0];
						for (int32_t i = 0; i < gltfimage.width * gltfimage.height; ++i) {
							for (int32_t j = 0; j < 3; ++j) {
								rgba[j] = rgb[j];
							}
							rgba += 4;
							rgb += 3;
						}
						deleteBuffer = true;*/
					}
					else
					{
						imageData->Width = curImage.width;
						imageData->Height = curImage.height;
						imageData->NrChannels = curImage.component;
						imageData->Data = new unsigned char[curImage.image.size()];
						memcpy(imageData->Data, curImage.image.data(), curImage.image.size());
						imageData->Size = curImage.image.size();
						/* stbi_write_png("Cube.png", imageData.Width, imageData.Height, imageData.NrChannels,
										imageData.Data, imageData.Width * imageData.NrChannels);*/
					}
					imageData;
					break;
				}
				default:
					std::cout << "Current bits per channel isnt supported" << std::endl;
				}
			}
		}
	}

	void Scene::LoadNodes(tinygltf::Model* model,
		glm::ivec2* imageSize)
	{
		for (int nodeIndex = 0; nodeIndex < model->nodes.size(); nodeIndex++)
		{
			tinygltf::Node* curNode = &model->nodes[nodeIndex];
			glm::mat4 nodeMatrix = GetNodeMatrix(curNode);
			LoadNode(model, curNode, glm::mat4(1.0), imageSize);

			for (int childNode = 0; childNode < curNode->children.size(); childNode++)
			{
				int index = curNode->children[childNode];
				tinygltf::Node* children = &model->nodes[index];
				if (index < m_Instances.size() - 1 && !m_Instances[index].Name.empty())
				{
					Transform& transform = m_Transforms[m_Instances[index].transformID];
					transform.ModelMatrix = GetNodeMatrix(curNode) * transform.ModelMatrix;
					transform.InvModelMatrix = glm::inverse(transform.ModelMatrix);
				}
				else
				{
					LoadNode(model, children, nodeMatrix, imageSize);
				}

				nodeIndex++;
			}
		}

		LogInfo("Total nodes: " + std::to_string(m_Instances.size()));
	}

    void Scene::LoadVertices(tinygltf::Model& model,
                             tinygltf::Primitive& primitive,
                             std::vector<Vertex>* vertices,
                             BoundingBox* bb)
    {
        for(auto& attribute : primitive.attributes)
        {
            auto index = attribute.second;
            tinygltf::Accessor* attribAccessor = &model.accessors[index];
            auto& bufferView = model.bufferViews[attribAccessor->bufferView];
            auto& buffer = model.buffers[bufferView.buffer];

			//resize it once
			if (vertices->size() == 0)
			{
				vertices->resize(attribAccessor->count);
				bb->Reset();
			}

			auto currentPtr = bufferView.byteOffset + attribAccessor->byteOffset;

			if (attribute.first == "POSITION")
			{
				for (auto i = 0; i < attribAccessor->count; i++)
				{
					auto size = std::max(bufferView.byteStride, sizeof(Vertex::Position));
					memcpy(&(*vertices)[i].Position,
						buffer.data.data() + currentPtr + sizeof(Vertex::Position) * i,
						size);

					bb->Max = glm::max(bb->Max, (*vertices)[i].Position);
					bb->Min = glm::min(bb->Min, (*vertices)[i].Position);
				}
			}

			else if (attribute.first == "NORMAL")
			{
				for (auto i = 0; i < attribAccessor->count; i++)
				{
					auto size = std::max(bufferView.byteStride, sizeof(Vertex::Normal));
					memcpy(&(*vertices)[i].Normal,
						buffer.data.data() + currentPtr + sizeof(Vertex::Normal) * i,
						size);
				}
			}
			else if (attribute.first == "TEXCOORD_0")
			{
				for (auto i = 0; i < attribAccessor->count; i++)
				{
					auto size = std::max(bufferView.byteStride, sizeof(Vertex::TexCoords0));
					memcpy(&(*vertices)[i].TexCoords0,
						buffer.data.data() + currentPtr + sizeof(Vertex::TexCoords0) * i,
						size);
				}
			}
			else
			{
				MsgAssertIfTrue(true, "Attribute with of type: " + attribute.first + " isn`t supported");
			}
		}
	}

	Material Scene::LoadMaterial(tinygltf::Model& model,
		tinygltf::Primitive& primitive,
		const std::string& modelPath)
	{
		if (primitive.material == -1)
		{
			return {};
		}

		Material material;

		tinygltf::Material curMat = model.materials[primitive.material];

		if (m_Materials.count(primitive.material) != 0)
		{
			LogInfo("Found the same material");
			//__debugbreak();
			return m_Materials[primitive.material];
		}

		material.emissiveFactor = glm::make_vec3(curMat.emissiveFactor.data());
		material.alphaMode = OPAQ;
		material.alphaCutoff = curMat.alphaCutoff;
		material.doubleSided = curMat.doubleSided;
		material.baseColorFactor = glm::make_vec4(curMat.pbrMetallicRoughness.baseColorFactor.data());
		material.metallicFactor = curMat.pbrMetallicRoughness.metallicFactor;
		material.roughtnessFactor = curMat.pbrMetallicRoughness.roughnessFactor;
		material.normalTextureScale = curMat.normalTexture.scale;
		material.occlusionTextureStrenght = curMat.occlusionTexture.strength;

		bool hasBaseColorTexture = curMat.pbrMetallicRoughness.baseColorTexture.index != -1;
		if (hasBaseColorTexture && m_Textures.count(curMat.pbrMetallicRoughness.baseColorTexture.index) == 0)
		{
			util::ImageData data;
			LoadTexture(model, curMat.pbrMetallicRoughness.baseColorTexture.index, modelPath, &data);
			m_Textures[curMat.pbrMetallicRoughness.baseColorTexture.index] = { data };
			material.BaseColor = &m_Textures[curMat.pbrMetallicRoughness.baseColorTexture.index];
		}
		else if (m_Textures.count(curMat.pbrMetallicRoughness.baseColorTexture.index) != 0)
		{
			material.Normal = &m_Textures[curMat.normalTexture.index];
		}

		bool hasMetallicRoughnessTexture = curMat.pbrMetallicRoughness.metallicRoughnessTexture.index != -1;
		if (hasMetallicRoughnessTexture && m_Textures.count(curMat.pbrMetallicRoughness.metallicRoughnessTexture.index) == 0)
		{
			util::ImageData data;
			LoadTexture(model, curMat.pbrMetallicRoughness.metallicRoughnessTexture.index, modelPath, &data);
			m_Textures[curMat.pbrMetallicRoughness.metallicRoughnessTexture.index] = { data };
			material.MetallicRoughness = &m_Textures[curMat.pbrMetallicRoughness.metallicRoughnessTexture.index];
		}
		else if (m_Textures.count(curMat.pbrMetallicRoughness.metallicRoughnessTexture.index) != 0)
		{
			material.Normal = &m_Textures[curMat.normalTexture.index];
		}

		bool hasNormalTexture = curMat.normalTexture.index != -1;
		if (hasNormalTexture && m_Textures.count(curMat.normalTexture.index) == 0)
		{
			util::ImageData data;
			LoadTexture(model, curMat.normalTexture.index, modelPath, &data);
			m_Textures[curMat.normalTexture.index] = { data };
			material.Normal = &m_Textures[curMat.normalTexture.index];
		}
		else if (m_Textures.count(curMat.normalTexture.index) != 0)
		{
			material.Normal = &m_Textures[curMat.normalTexture.index];
		}

		bool hasOcclusionTexture = curMat.occlusionTexture.index != -1;
		if (hasOcclusionTexture && m_Textures.count(curMat.occlusionTexture.index) == 0)
		{
			util::ImageData data;
			LoadTexture(model, curMat.occlusionTexture.index, modelPath, &data);
			m_Textures[curMat.occlusionTexture.index] = { data };
			material.Occlusion = &m_Textures[curMat.occlusionTexture.index];
		}
		else if (m_Textures.count(curMat.occlusionTexture.index))
		{
			material.Occlusion = &m_Textures[curMat.occlusionTexture.index];
		}

		return material;
	}

    void Scene::LoadIndices(tinygltf::Model& model,
                            tinygltf::Primitive& primitive,
                            const std::vector<Vertex>& vertices,
                            std::vector<Triangle>* triangles)
    {
        if(primitive.indices > -1)
        {
            tinygltf::Accessor* indicesAccessor = &model.accessors[primitive.indices];

			auto& bufferView = model.bufferViews[indicesAccessor->bufferView];
			auto& buffer = model.buffers[bufferView.buffer];

			triangles->resize(indicesAccessor->count / 3);
			auto lenghtInBytes = (bufferView.byteLength / indicesAccessor->count) * 3;

			switch (indicesAccessor->componentType)
			{
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
			{
				for (int i = 0; i < triangles->size(); i++)
				{
					std::array<uint8_t, 3> indices;
					memcpy(indices.data(),
						buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
						3 * sizeof(uint8_t));
					(*triangles)[i].Indices[0] = static_cast<unsigned int>(indices[0]);
					(*triangles)[i].Indices[1] = static_cast<unsigned int>(indices[1]);
					(*triangles)[i].Indices[2] = static_cast<unsigned int>(indices[2]);
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
			{
				for (int i = 0; i < triangles->size(); i++)
				{
					std::array<uint16_t, 3> indices;
					memcpy(indices.data(),
						buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
						3 * sizeof(uint16_t));
					(*triangles)[i].Indices[0] = static_cast<unsigned int>(indices[0]);
					(*triangles)[i].Indices[1] = static_cast<unsigned int>(indices[1]);
					(*triangles)[i].Indices[2] = static_cast<unsigned int>(indices[2]);
				}
				break;
			}
			case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
			{
				for (int i = 0; i < triangles->size(); i++)
				{
					std::array<uint32_t, 3> indices;
					memcpy(indices.data(),
						buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
						3 * sizeof(uint32_t));
					(*triangles)[i].Indices[0] = static_cast<unsigned int>(indices[0]);
					(*triangles)[i].Indices[1] = static_cast<unsigned int>(indices[1]);
					(*triangles)[i].Indices[2] = static_cast<unsigned int>(indices[2]);
				}
				break;
			}
			}
		}

		for (auto& t : *triangles)
		{
			t.Normal = (vertices[t.Indices[0]].Normal +
				vertices[t.Indices[1]].Normal +
				vertices[t.Indices[2]].Normal) / 3.0f;
		}
	}

	glm::mat4 Scene::GetNodeMatrix(tinygltf::Node* node)
	{
		glm::dmat4 matrix = glm::dmat4(1.0);

		if (!node->matrix.empty())
		{
			matrix = glm::mat4(glm::make_mat4(node->matrix.data()));
		}
		else
		{
			if (node->translation.size() != 0)
			{
				matrix = glm::translate(matrix, glm::make_vec3(node->translation.data()));
			}

			if (node->rotation.size() != 0)
			{
				//XYZW -> WXYZ
				float data[] =
				{
					node->rotation[3],
					node->rotation[0] ,
					node->rotation[1] ,
					node->rotation[2]
				};

				glm::quat q = glm::make_quat(data);
				glm::mat4 quat = glm::mat4_cast(q);
				matrix = matrix * glm::dmat4(quat);
			}

			if (node->scale.size() != 0)
			{
				matrix = glm::scale(matrix, glm::make_vec3(node->scale.data()));
			}
		}

		return matrix;
	}

	void Scene::Render(Film* film)
	{
		auto processor_count = std::thread::hardware_concurrency();
		if (processor_count == 0)
		{
			processor_count = 1;
		}

		int columnsPerBatch = film->Width / processor_count;

		std::vector<std::thread> workers;
		workers.reserve(processor_count);

        auto batchProcessor = [this, film](int startRow, int endRow, int batchIndex)
        {
            //EASY_THREAD("Render Thread");

            float xStep = 1.0f / static_cast<float>(film->Width);
            float yStep = 1.0f / static_cast<float>(film->Height);

            for(int x = startRow; x < endRow; x++)
            {
                //LogInfo("Row finished: " + std::to_string(x));
                for(int y = 0; y < film->Height; y++)
                {
                    EASY_BLOCK("ROW");
                    film->SetPixelColor({ x,y }, GetPixelColor(x * xStep, 1.0 - (y * yStep)));
                    EASY_END_BLOCK;
                }
            }

            LogInfo("Finished worker nr: " + std::to_string(batchIndex));
            //EASY_END_BLOCK;
        };

		int currentRow = 0;
		for (int i = 0; i < processor_count; i++)
		{
			LogInfo("Started worker nr: " +
				std::to_string(i)
				+ " with range ["
				+ std::to_string(currentRow)
				+ " "
				+ std::to_string(currentRow + columnsPerBatch)
				+ "]");

			workers.push_back(std::thread(batchProcessor, currentRow, currentRow + columnsPerBatch, i));
			currentRow += columnsPerBatch;
		}

		if (currentRow < film->Width)
		{
			LogInfo("Started worker nr: " +
				std::to_string(processor_count + 1)
				+ " with range ["
				+ std::to_string(currentRow)
				+ " "
				+ std::to_string(film->Width)
				+ "]");

			workers.push_back(std::thread(batchProcessor, currentRow, film->Height, processor_count + 1));
		}

		for (auto& worker : workers)
		{
			worker.join();
		}
	}

	glm::vec3 Scene::GetPixelColor(float x, float y)
	{
		Ray ray = { m_Cameras[0].Position(),
					glm::normalize(m_Cameras[0].PixelPos(x, y) - m_Cameras[0].Position()),
					std::numeric_limits<float>::infinity(), GRAY };

        for(int i = 0; i < m_Instances.size(); i++)
        {
            Instance& instance = m_Instances[i];
            Model& model = m_Models[instance.modelID];
            const Transform& tr = m_Transforms[instance.transformID];
            Ray localRay{ tr.InvModelMatrix * glm::vec4(ray.Position, 1.0),
                          glm::normalize(tr.InvModelMatrix * glm::vec4(ray.Direction, 0.0)),
                          ray.Distance, ray.Color };

            if(model.FindIntersection(&localRay))
            {
                ray.Distance = localRay.Distance;
                //ray.Color = glm::vec3(1.0, 0, 0);

               /* MeshPrimitive& primitive = model.Primitives[localRay.PrimitiveId];
                std::vector<Vertex>& Vertices = primitive.Vertices;
                Triangle& Triangle = primitive.Triangles[localRay.TriangleId];*/

                /*if(localRay.PrimitiveId < 1)
                { 
                    continue;
                }*/

                MeshPrimitive& primitive = model.Primitives[localRay.PrimitiveId];
                Material& material = m_Materials[primitive.MaterialID];
                ray.Color = material.baseColorFactor + glm::vec4(material.emissiveFactor, 0.0);

                /*
                 glm::vec2 interpolatedTexCoords = (Vertices[Triangle.Indices[0]].TexCoords0 * localRay.Barycentric.x +
                                                    Vertices[Triangle.Indices[1]].TexCoords0 * localRay.Barycentric.y +
                                                    Vertices[Triangle.Indices[2]].TexCoords0 * (1.0f - localRay.Barycentric.x - localRay.Barycentric.y));

                if(m_Materials[instance.materialID].BaseColor == nullptr ||
                    m_Materials[instance.materialID].BaseColor->ImageData.Data == nullptr)
                 {

                     continue;
                 }

                 util::ImageData& ImageData = m_Materials[instance.materialID].BaseColor->ImageData;

                 glm::ivec2 texCoordsInPixel = { ImageData.Width * interpolatedTexCoords.x,
                     ImageData.Height * interpolatedTexCoords.y };
                 texCoordsInPixel.x %= ImageData.Width;
                 texCoordsInPixel.y %= ImageData.Height;

                 int pixelIndex = (texCoordsInPixel.y * ImageData.Width + texCoordsInPixel.x) * ImageData.NrChannels;
                 ray.Color[0] = ImageData.Data[pixelIndex + 0], 1.0f;
                 ray.Color[1] = ImageData.Data[pixelIndex + 1], 1.0f;
                 ray.Color[2] = ImageData.Data[pixelIndex + 2], 1.0f; */
            }
        }

        return ray.Color * 255.0f;
    }
}

