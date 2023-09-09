#include "Scene.h"

namespace pbr
{
    Scene::Scene()
    {

    }

    void Scene::Init(tinygltf::Model* model, float height)
    {
        for(auto& scene : model->scenes)
        {
            for(auto& node : scene.nodes)
            {
                tinygltf::Node* curNode = &model->nodes[node];
                //if out node is camera
                if(curNode->camera != -1)
                {
                    tinygltf::Camera* camera = &model->cameras[curNode->camera];
                    if(camera->type == "perspective")
                    {
                        glm::mat4 viewMatrix = GetNodeMatrix(curNode);
                        float yFov = camera->perspective.yfov;
                        float aspectRatio = camera->perspective.aspectRatio;
                        float znear = camera->perspective.znear;
                        float zfar = camera->perspective.zfar;

                        float width = height * aspectRatio;
                        float fovDegres = glm::degrees(yFov);

                        m_Cameras.emplace_back(Camera(width, height, fovDegres, znear, zfar, viewMatrix));
                    }
                }
                else
                {
                    //otherwise is point light source
                    if(curNode->light != -1)
                    {
                        PointLight light{};
                        light.WorldMatrix = GetNodeMatrix(curNode);
                        m_PointLights.emplace_back(light);
                        continue;
                    }

                    //otherwise is mesh
                    

                    //parse only meshes for now
                    if(curNode->mesh != -1)
                    {
                        tinygltf::Mesh* curMesh = &model->meshes[curNode->mesh];
                        Mesh mesh;
                        mesh.Name = curMesh->name;
                        mesh.ModelMatrix = GetNodeMatrix(curNode);
                        std::vector<Vertex>& vertices = mesh.Vertices;
                        std::vector<Triangle>& triangles = mesh.Triangles;
                        m_Meshes.push_back(mesh);

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
                                auto lenghtInBytes = bufferView.byteLength / attribAccessor->count;

                                //resize it once
                                if(vertices.size() == 0)
                                {
                                    vertices.resize(attribAccessor->count);
                                }

                                if(attribute.first == "POSITION")
                                {
                                    for(int i = 0; i < vertices.size(); i++)
                                    {
                                        memcpy(&vertices[i].Position,
                                               buffer.data.data() +
                                               bufferView.byteOffset + lenghtInBytes * i,
                                               sizeof(Vertex::Position));
                                    }
                                }

                                else if(attribute.first == "NORMAL")
                                {
                                    for(int i = 0; i < vertices.size(); i++)
                                    {
                                        memcpy(&vertices[i].Normal,
                                               buffer.data.data() +
                                               bufferView.byteOffset + lenghtInBytes * i,
                                               sizeof(Vertex::Normal));
                                    }
                                }
                                else if(attribute.first == "TEXCOORD_0")
                                {
                                    for(int i = 0; i < vertices.size(); i++)
                                    {
                                        memcpy(&vertices[i].TexCoords,
                                               buffer.data.data() +
                                               bufferView.byteOffset + lenghtInBytes * i,
                                               sizeof(Vertex::TexCoords));
                                    }
                                }
                                else
                                {
                                    MsgAssertIfTrue(true, "Attribute with of type: " + attribute.first + " isn`t supported");
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void Scene::Render()
    {

    }

    glm::mat4 Scene::GetNodeMatrix(tinygltf::Node* node)
    {
        glm::mat4 matrix = glm::mat4(1.0);

        if(!node->matrix.empty())
        {
            matrix = { node->matrix[0], node->matrix[1], node->matrix[2], node->matrix[3],
                           node->matrix[4], node->matrix[5], node->matrix[6], node->matrix[7],
                           node->matrix[8], node->matrix[9], node->matrix[10], node->matrix[11],
                           node->matrix[12], node->matrix[13], node->matrix[14], node->matrix[15] };
        }
        else
        {
            if(node->translation.size() != 0)
            {
                matrix = glm::translate(matrix,
                                        { node->translation[0],
                                          node->translation[1],
                                          node->translation[2] });
            }

            if(node->rotation.size() != 0)
            {
                glm::quat q;
                q.x = node->rotation[0];
                q.y = node->rotation[1];
                q.z = node->rotation[2];
                q.w = node->rotation[3];
                glm::quat quadMat = glm::quat_cast(matrix);
                q = q * quadMat;

                matrix = glm::mat4_cast(q);
            }

            if(node->scale.size() != 0)
            {
                matrix = glm::scale(matrix, { node->scale[0],
                                          node->scale[1],
                                          node->scale[2] });
            }
        }

        return matrix;
    }
}

