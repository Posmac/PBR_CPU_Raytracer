#include "Scene.h"
#include <fstream>

namespace pbr
{
    Scene::Scene()
    {

    }

    void Scene::Init(tinygltf::Model* model, glm::ivec2* imageSize)
    {
        for(int node = 0; node < model->nodes.size(); node++)
        {
            tinygltf::Node* curNode = &model->nodes[node];
            glm::mat4 nodeMatrix = GetNodeMatrix(curNode);
            LoadNode(model, curNode, glm::mat4(1.0), imageSize);
            if(!curNode->children.empty())
            {
                for(int childNode = 0; childNode < curNode->children.size(); childNode++)
                {
                    int index = curNode->children[childNode];
                    LoadNode(model, &model->nodes[index], nodeMatrix, imageSize);
                    node++;
                }
            }
        }

        if(m_Cameras.empty())
        {
            LogInfo("Gltf file dont contain any camera, created basic one");
            glm::mat4 view = glm::lookAtRH(glm::vec3(0, 0, -1), glm::vec3(0), glm::vec3(0, 1, 0));
            imageSize->x = WIDTH;
            imageSize->y = HEIGHT;
            m_Cameras.emplace_back(Camera(glm::radians(60.0f), 0.1f, 1000.0f, *imageSize, view));

        }
    }

    void Scene::LoadNode(tinygltf::Model* model, tinygltf::Node* node, const glm::mat4& parentNodeMatrix, glm::ivec2* imageSize)
    {
        //if out node is camera
        if(node->camera != -1)
        {
            tinygltf::Camera* camera = &model->cameras[node->camera];
            if(camera->type == "perspective")
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
            if(node->light != -1)
            {
                PointLight light{};
                light.WorldMatrix = GetNodeMatrix(node);
                m_PointLights.emplace_back(light);
                return;
            }

            //otherwise is mesh
            //parse only meshes for now
            if(node->mesh != -1)
            {
                tinygltf::Mesh* curMesh = &model->meshes[node->mesh];
                std::vector<Vertex> vertices;
                std::vector<Triangle> triangles;

                std::unordered_set<glm::vec3> uniquePos;

                for(auto& primitive : curMesh->primitives)
                {
                    //render mode
                    int mode = primitive.mode;
                    //load indices 
                    if(primitive.indices > -1)
                    {
                        tinygltf::Accessor* indicesAccessor = &model->accessors[primitive.indices];

                        auto& bufferView = model->bufferViews[indicesAccessor->bufferView];
                        auto& buffer = model->buffers[bufferView.buffer];

                        triangles.resize(indicesAccessor->count / 3);
                        auto lenghtInBytes = (bufferView.byteLength / indicesAccessor->count) * 3;

                        switch(indicesAccessor->componentType)
                        {
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                            {
                                for(int i = 0; i < triangles.size(); i++)
                                {
                                    std::array<uint8_t, 3> indices;
                                    memcpy(indices.data(),
                                           buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
                                           3 * sizeof(uint8_t));
                                    triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
                                    triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
                                    triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
                                    triangles[i].Color = glm::linearRand(glm::vec3(0.2), glm::vec3(1.0));
                                }
                                break;
                            }
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                            {
                                for(int i = 0; i < triangles.size(); i++)
                                {
                                    std::array<uint16_t, 3> indices;
                                    memcpy(indices.data(),
                                           buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
                                           3 * sizeof(uint16_t));
                                    triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
                                    triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
                                    triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
                                    triangles[i].Color = glm::linearRand(glm::vec3(0.2), glm::vec3(1.0));
                                }
                                break;
                            }
                            case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                            {
                                for(int i = 0; i < triangles.size(); i++)
                                {
                                    std::array<uint32_t, 3> indices;
                                    memcpy(indices.data(),
                                           buffer.data.data() + bufferView.byteOffset + lenghtInBytes * i,
                                           3 * sizeof(uint32_t));
                                    triangles[i].Indices[0] = static_cast<unsigned int>(indices[0]);
                                    triangles[i].Indices[1] = static_cast<unsigned int>(indices[1]);
                                    triangles[i].Indices[2] = static_cast<unsigned int>(indices[2]);
                                    triangles[i].Color = glm::linearRand(glm::vec3(0.8), glm::vec3(1.0));
                                }
                                break;
                            }
                        }
                    }

                    //load vertices
                    for(auto& attribute : primitive.attributes)
                    {
                        auto index = attribute.second;
                        tinygltf::Accessor* attribAccessor = &model->accessors[index];
                        auto& bufferView = model->bufferViews[attribAccessor->bufferView];
                        auto& buffer = model->buffers[bufferView.buffer];

                        //resize it once
                        if(vertices.size() == 0)
                        {
                            vertices.resize(attribAccessor->count);
                        }

                        auto currentPtr = bufferView.byteOffset + attribAccessor->byteOffset;

                        if(attribute.first == "POSITION")
                        {
                            for(auto i = 0; i < attribAccessor->count; i++)
                            {
                                auto size = std::max(bufferView.byteStride, sizeof(Vertex::Position));
                                memcpy(&vertices[i].Position,
                                       buffer.data.data() + currentPtr + sizeof(Vertex::Position) * i,
                                       size);
                                //currentPtr += bufferView.byteStride;
                                //LogVec3(vertices[i].Position);
                            }
                        }

                        else if(attribute.first == "NORMAL")
                        {
                            for(auto i = 0; i < attribAccessor->count; i++)
                            {
                                auto size = std::max(bufferView.byteStride, sizeof(Vertex::Normal));
                                memcpy(&vertices[i].Normal,
                                       buffer.data.data() + currentPtr + sizeof(Vertex::Normal) * i,
                                       size);
                                //currentPtr += bufferView.byteStride;
                               //LogVec3(vertices[i].Normal);
                            }
                        }
                        else if(attribute.first == "TEXCOORD_0")
                        {
                            for(auto i = 0; i < attribAccessor->count; i++)
                            {
                                auto size = std::max(bufferView.byteStride, sizeof(Vertex::TexCoords));
                                memcpy(&vertices[i].TexCoords,
                                       buffer.data.data() + currentPtr + sizeof(Vertex::TexCoords) * i,
                                       size);
                                //currentPtr += bufferView.byteStride;
                                //LogVec2(vertices[i].TexCoords);
                            }
                        }
                        else
                        {
                            MsgAssertIfTrue(true, "Attribute with of type: " + attribute.first + " isn`t supported");
                        }
                    }
                }

                Mesh mesh;
                mesh.Name = curMesh->name;
                mesh.ModelMatrix = parentNodeMatrix * GetNodeMatrix(node);
                mesh.InvModelMatrix = glm::inverse(mesh.ModelMatrix);
                /*for(auto& v : vertices)
                {
                    LogVec3(v.Position);
                    LogVec3(v.Normal);
                    LogVec2(v.TexCoords);
                    std::cout << std::endl;
                }
                for(auto& t : triangles)
                {
                    LogVec3({ t.Indices[0], t.Indices[1], t.Indices[2] });
                    LogVec3(t.Color);
                    std::cout << std::endl;
                }*/
                mesh.Vertices = std::move(vertices);
                mesh.Triangles = std::move(triangles);
                m_Meshes.push_back(std::move(mesh));
            }
        }
    }

    glm::mat4 Scene::GetNodeMatrix(tinygltf::Node* node)
    {
        glm::dmat4 matrix = glm::dmat4(1.0);

        if(!node->matrix.empty())
        {
            matrix = glm::mat4(glm::make_mat4(node->matrix.data()));
        }
        else
        {
            if(node->translation.size() != 0)
            {
                matrix = glm::translate(matrix, glm::make_vec3(node->translation.data()));
            }

            if(node->rotation.size() != 0)
            {
                matrix *= glm::toMat4(glm::make_quat(node->rotation.data()));
            }

            if(node->scale.size() != 0)
            {
                matrix = glm::scale(matrix, glm::make_vec3(node->scale.data()));
            }
        }

        return matrix;
    }

    void Scene::Render(Film* film)
    {
        //std::ofstream stream;
        //stream.open("RESULT_IMAGE.ppm");

        //if(!stream.is_open())
        //{
        //    std::cout << "FFFF\n";
        //}

        //stream << "P3\n" << film->Width << ' ' << film->Height << "\n" << "255\n";
        //LogInfo("PPM");
        //for(int y = 0; y < film->Height; y++)
        //{
        //    LogInfo("Row nr " + std::to_string(y));
        //    for(int x = 0; x < film->Width; x++)
        //    {
        //        float xStep = static_cast<float>(x) / static_cast<float>(film->Width);
        //        float yStep = static_cast<float>(y) / static_cast<float>(film->Height);
        //        glm::vec4 color = /*{ xStep, yStep, 0, 1 }*/ GetPixelColor(xStep, yStep);
        //        glm::ivec4 ncolor = color * 255.0f;
        //        color = glm::clamp(ncolor, glm::ivec4(0), glm::ivec4(255));
        //        stream << ncolor.x << ' ' << ncolor.y << ' ' << ncolor.z << "\n";
        //    }
        //}
        //stream.close();

        LogInfo("PNG");
        for(int x = 0; x < film->Width; x++)
        {
            LogInfo("Row nr " + std::to_string(x));
            for(int y = 0; y < film->Height; y++)
            {
                float xStep = static_cast<float>(x) / static_cast<float>(film->Width);
                float yStep = static_cast<float>(film->Height - y) / static_cast<float>(film->Height);
                glm::vec4 color = /*{ xStep, yStep, 0, 1 }*/ GetPixelColor(xStep, yStep);
                film->SetPixelColor({ x,y }, color);
            }
        }
    }

    glm::vec4 Scene::GetPixelColor(float x, float y)
    {
        Ray ray{};
        ray.Position = m_Cameras[0].Position();
        ray.Direction = glm::normalize(m_Cameras[0].PixelPos(x, y) - ray.Position);
        ray.Distance = std::numeric_limits<float>::infinity();

        for(auto& mesh : m_Meshes)
        {
            if(mesh.FindIntersection(&ray))
            {
                return { ray.Color, 1.0 };
            }
        }

        return { 0.0, 0.0, 0.0, 1.0 };
    }
}

