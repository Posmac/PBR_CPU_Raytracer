#include "Camera.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace pbr
{
    Camera::Camera(float aspectRatio,
                   float fov, 
                   float near, 
                   float far,
                   const glm::mat4& viewMatrix)
    {
        m_AspectRatio = aspectRatio;
        m_Fov = fov;
        m_Near = near;
        m_Far = far;

        m_ViewMatrix = glm::inverse(viewMatrix);
        m_ProjectionMatrix = glm::perspectiveRH_ZO(fov, aspectRatio, near, far);

        m_ViewProjMatrix = m_ProjectionMatrix * m_ViewMatrix;
        m_InvViewMatrix = glm::inverse(m_ViewMatrix);
        m_InvProjMatrix = glm::inverse(m_ProjectionMatrix);
        m_InvViewProjMatrix = glm::inverse(m_ViewProjMatrix);

        m_BlCornerNdc = glm::vec4(-1, -1, 0, 1);
        m_TlCornerNdc = glm::vec4( 1, -1, 0, 1);
        m_BrCornerNdc = glm::vec4(-1,  1, 0, 1);

        m_BlCornerWorld = m_InvViewProjMatrix * m_BlCornerNdc;
        m_TlCornerWorld = m_InvViewProjMatrix * m_TlCornerNdc;
        m_BrCornerWorld = m_InvViewProjMatrix * m_BrCornerNdc;

        m_BlCornerWorld = m_BlCornerWorld / m_BlCornerWorld.w;
        m_TlCornerWorld = m_TlCornerWorld / m_TlCornerWorld.w;
        m_BrCornerWorld = m_BrCornerWorld / m_BrCornerWorld.w;

        m_RightDirection = m_BrCornerWorld - m_BlCornerWorld;
        m_UpDirection = m_TlCornerWorld - m_BlCornerWorld;
    }

    glm::vec3 Camera::PixelPos(float x, float y)
    {
        return m_BlCornerWorld + m_RightDirection * x + m_UpDirection * y;
    }

    glm::vec3 Camera::Position()
    {
        return m_InvViewMatrix[3];
    }
}