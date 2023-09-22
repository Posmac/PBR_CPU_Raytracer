#include "Scene.h"

#include <thread>

namespace pbr
{
	Scene::Scene()
	{

	}

	void Scene::Init(tinygltf::Model* model, glm::ivec2* imageSize)
	{
		for (int node = 0; node < model->nodes.size(); node++)
		{
			tinygltf::Node* curNode = &model->nodes[node];
			glm::mat4 nodeMatrix = GetNodeMatrix(curNode);
			LoadNode(model, curNode, glm::mat4(1.0), imageSize);
			if (!curNode->children.empty())
			{
				for (int childNode = 0; childNode < curNode->children.size(); childNode++)
				{
					int index = curNode->children[childNode];
					LoadNode(model, &model->nodes[index], nodeMatrix, imageSize);
					node++;
				}
			}
		}

		if (m_Cameras.empty())
		{
			LogInfo("Gltf file dont contain any camera, created the basic one");
			glm::mat4 view = glm::lookAtRH(glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0, 1, 0));
			imageSize->x = WIDTH;
			imageSize->y = HEIGHT;
			m_Cameras.emplace_back(Camera(glm::radians(60.0f), 0.1f, 1000.0f, *imageSize, view));

		}
	}

	void Scene::LoadNode(tinygltf::Model* model, tinygltf::Node* node, const glm::mat4& parentNodeMatrix, glm::ivec2* imageSize)
	{
		//if out node is camera
		if (node->camera != -1)
		{
			tinygltf::Camera* camera = &model->cameras[node->camera];
			if (camera->type == "perspective")
			{
				float yFov = camera->perspective.yfov;
				float aspectRatio = camera->perspective.aspectRatio;
				float znear = camera->perspective.znear;
				float zfar = camera->perspective.zfar;

				glm::mat4 view = parentNodeMatrix * GetNodeMatrix(node);
				imageSize->x = imageSize->y * aspectRatio;
				m_Cameras.emplace_back(Camera(yFov, znear, zfar, *imageSize, glm::inverse(view)));
			}
		}
		else
		{
			//otherwise is point light source
			if (node->light != -1)
			{
				PointLight light{};
				light.WorldMatrix = GetNodeMatrix(node);
				m_PointLights.emplace_back(light);
				return;
			}

			//otherwise is mesh
			//parse only meshes for now
			if (node->mesh != -1)
			{
				Mesh mesh;
				tinygltf::Mesh* curMesh = &model->meshes[node->mesh];
				std::vector<Vertex> vertices;
				std::vector<Triangle> triangles;
				std::unordered_set<glm::vec3> uniquePos;

				for (auto& primitive : curMesh->primitives)
				{
					//render mode
					int mode = primitive.mode;
					//load indices 
					if (primitive.indices > -1)
					{
						tinygltf::Accessor* indicesAccessor = &model->accessors[primitive.indices];

						auto& bufferView = model->bufferViews[indicesAccessor->bufferView];
						auto& buffer = model->buffers[bufferView.buffer];

						triangles.resize(indicesAccessor->count / 3);
						auto lenghtInBytes = (bufferView.byteLength / indicesAccessor->count) * 3;

						switch (indicesAccessor->componentType)
						{
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
						{
							for (int i = 0; i < triangles.size(); i++)
							{
								std::array<uint8_t, 3> indices;
								memcpy(indices.data(),
									buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
									3 * sizeof(uint8_t));
								triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
								triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
								triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
								triangles[i].Color = glm::vec4(glm::linearRand(glm::vec3(0.3), glm::vec3(1.0)), 1.0);
							}
							break;
						}
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
						{
							for (int i = 0; i < triangles.size(); i++)
							{
								std::array<uint16_t, 3> indices;
								memcpy(indices.data(),
									buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
									3 * sizeof(uint16_t));
								triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
								triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
								triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
								triangles[i].Color = glm::vec4(glm::linearRand(glm::vec3(0.3), glm::vec3(1.0)), 1.0);
							}
							break;
						}
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
						{
							for (int i = 0; i < triangles.size(); i++)
							{
								std::array<uint32_t, 3> indices;
								memcpy(indices.data(),
									buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
									3 * sizeof(uint32_t));
								triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
								triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
								triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
								triangles[i].Color = glm::vec4(glm::linearRand(glm::vec3(0.3), glm::vec3(1.0)), 1.0);
							}
							break;
						}
						}
					}

					//load vertices
					for (auto& attribute : primitive.attributes)
					{
						auto index = attribute.second;
						tinygltf::Accessor* attribAccessor = &model->accessors[index];
						auto& bufferView = model->bufferViews[attribAccessor->bufferView];
						auto& buffer = model->buffers[bufferView.buffer];

						//resize it once
						if (vertices.size() == 0)
						{
							vertices.resize(attribAccessor->count);
						}

						auto currentPtr = bufferView.byteOffset + attribAccessor->byteOffset;

						if (attribute.first == "POSITION")
						{
							for (auto i = 0; i < attribAccessor->count; i++)
							{
								auto size = std::max(bufferView.byteStride, sizeof(Vertex::Position));
								memcpy(&vertices[i].Position,
									buffer.data.data() + currentPtr + sizeof(Vertex::Position) * i,
									size);
							}
						}

						else if (attribute.first == "NORMAL")
						{
							for (auto i = 0; i < attribAccessor->count; i++)
							{
								auto size = std::max(bufferView.byteStride, sizeof(Vertex::Normal));
								memcpy(&vertices[i].Normal,
									buffer.data.data() + currentPtr + sizeof(Vertex::Normal) * i,
									size);
							}
						}
						else if (attribute.first == "TEXCOORD_0")
						{
							for (auto i = 0; i < attribAccessor->count; i++)
							{
								auto size = std::max(bufferView.byteStride, sizeof(Vertex::TexCoords));
								memcpy(&vertices[i].TexCoords,
									buffer.data.data() + currentPtr + sizeof(Vertex::TexCoords) * i,
									size);
							}
						}
						else
						{
							MsgAssertIfTrue(true, "Attribute with of type: " + attribute.first + " isn`t supported");
						}
					}

					//load textures (for now only 1)
					if (primitive.material != -1)
					{
						tinygltf::Material curMat = model->materials[primitive.material];
						bool hasBaseColorTexture = curMat.pbrMetallicRoughness.baseColorTexture.index != -1;
						if (hasBaseColorTexture)
						{
							tinygltf::Texture curTexture = model->textures[curMat.pbrMetallicRoughness.baseColorTexture.index];
							if (curTexture.source != -1)
							{
								util::ImageData imageData{};
								tinygltf::Image curImage = model->images[curTexture.source];
								if (!curImage.uri.empty())
								{
									stbi_uc* data = stbi_load(curImage.uri.c_str(), &imageData.Width, &imageData.Height,
										&imageData.NrChannels, 0);
									if (data)
									{
										imageData.Data = data;
									}
								}
								else if (curImage.bufferView != -1)
								{
									switch (curImage.bits)
									{
										case 8:
										{
											auto& bufferView = model->bufferViews[curImage.bufferView];
											auto& buffer = model->buffers[bufferView.buffer];

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
												imageData.Width = curImage.width;
												imageData.Height = curImage.height;
												imageData.NrChannels = curImage.component;
												imageData.Data = new unsigned char[curImage.image.size()];
												memcpy(imageData.Data, curImage.image.data(), curImage.image.size());
												imageData.Size = curImage.image.size();
											}
											mesh.ImageData = imageData;
											break;
										}
										default:
											std::cout << "Current bits per channel isnt supported" << std::endl;
									}
								}
							}
						}
					}
				}

				mesh.Name = curMesh->name;
				mesh.ModelMatrix = parentNodeMatrix * GetNodeMatrix(node);
				mesh.InvModelMatrix = glm::inverse(mesh.ModelMatrix);
				mesh.Vertices = std::move(vertices);
				mesh.Triangles = std::move(triangles);

				for (auto& t : mesh.Triangles)
				{
					t.Normal = (mesh.Vertices[t.Indices[0]].Normal +
						mesh.Vertices[t.Indices[1]].Normal +
						mesh.Vertices[t.Indices[2]].Normal) / 3.0f;
				}

				m_Meshes.push_back(std::move(mesh));
			}
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
				matrix *= glm::toMat4(glm::make_quat(node->rotation.data()));
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
				float xStep = 1.0f / static_cast<float>(film->Width);
				float yStep = 1.0f / static_cast<float>(film->Height);

				for (int x = startRow; x < endRow; x++)
				{
					for (int y = 0; y < film->Height; y++)
					{
						film->SetPixelColor({ x,y }, GetPixelColor(x * xStep, 1.0 - (y * yStep)));
					}
				}

				LogInfo("Finished worker nr: " + std::to_string(batchIndex));
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
				+ "]\n");

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
				+ "]\n");

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
			std::numeric_limits<float>::infinity() };

		for (auto& mesh : m_Meshes)
		{
			if (mesh.FindIntersection(&ray))
			{
				return ray.Color;
			}
		}

		return BLACK;
	}
}

