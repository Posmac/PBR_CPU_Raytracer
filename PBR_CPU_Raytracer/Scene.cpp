#include "Scene.h"

namespace pbr
{
    Scene::Scene()
    {

    }

    void Scene::Init(tinygltf::Model* model)
    {
        if(model->cameras.size() == 0)
        {
            return;
        }

        //parse data
        {
            for(auto& node : model->nodes)
            {
                //setup cameras
                if(node.camera != -1)
                {
                    tinygltf::Camera camera = model->cameras[node.camera];
                    if(camera.type == "perspective")
                    {
                        if(node.matrix.empty())
                        {
                            //do nothing
                        }

                        glm::mat4 viewMatrix = { node.matrix[0], node.matrix[1], node.matrix[2], node.matrix[3],
                                                 node.matrix[4], node.matrix[5], node.matrix[6], node.matrix[7],
                                                 node.matrix[8], node.matrix[9], node.matrix[10], node.matrix[11],
                                                 node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15] };

                        float yFov = camera.perspective.yfov;
                        float aspectRatio = camera.perspective.aspectRatio;
                        float znear = camera.perspective.znear;
                        float zfar = camera.perspective.zfar;

                    }
                }
                //setup other objects
                else
                {

                }
            }

        }
    }

    void Scene::Render()
    {

    }
}

