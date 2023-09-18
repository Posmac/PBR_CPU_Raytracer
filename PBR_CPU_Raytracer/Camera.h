#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

namespace pbr
{
    class Camera
    {
    public:
        Camera(float fov = 60.0f, 
               float near = 0.1f, 
               float far = 100.0f,
               const glm::vec2& m_ScreenSize = glm::vec3(0),
               const glm::mat4& viewMatrix = glm::mat4(1.0));
        glm::vec4 PixelPos(float x, float y);
        glm::vec4 Position();

    private:
        glm::vec2 m_ScreenSize;
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