#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace pbr
{
    class Camera
    {
    public:
        Camera(int width = 1280.0, 
               int height = 720.0, 
               float fov = 60.0, 
               float near = 0.1f, 
               float far = 100.0f,
               const glm::vec3& eye = glm::vec3(0, -1, 0),
               const glm::vec3& lookAt = glm::vec3(0, 0, 0),
               const glm::vec3& up = glm::vec3(0, 1, 0));

    private:
        int m_Width;
        int m_Height;
        float m_Near;
        float m_Far;
        float m_Fov;//in degrees

        glm::mat4 m_ViewMatrix;
        glm::mat4 m_ProjectionMatrix;
        glm::mat4 m_ViewProjMatrix;

        glm::mat4 m_InvViewMatrix;
        glm::mat4 m_InvProjMatrix;
        glm::mat4 m_InvViewProjMatrix;

        glm::vec4 m_BlCornerNdc;
        glm::vec4 m_TlCornerNdc;
        glm::vec4 m_BrCornerNdc;
        glm::vec4 m_TrCornerNdc;

        glm::vec4 m_BlCornerWorld;
        glm::vec4 m_TlCornerWorld;
        glm::vec4 m_BrCornerWorld;
        glm::vec4 m_TrCornerWorld;
    };
}