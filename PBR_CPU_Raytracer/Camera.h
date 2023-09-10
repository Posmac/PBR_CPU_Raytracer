#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace pbr
{
    class Camera
    {
    public:
        Camera(float aspectRatio = 1.77f, 
               float fov = 60.0f, 
               float near = 0.1f, 
               float far = 100.0f,
               const glm::mat4& viewMatrix = glm::mat4(1.0));
        glm::vec3 PixelPos(float x, float y);
        glm::vec3 Position();

    private:
        float m_AspectRatio;
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

        glm::vec4 m_BlCornerWorld;
        glm::vec4 m_TlCornerWorld;
        glm::vec4 m_BrCornerWorld;

        glm::vec4 m_RightDirection;
        glm::vec4 m_UpDirection;
    };
}