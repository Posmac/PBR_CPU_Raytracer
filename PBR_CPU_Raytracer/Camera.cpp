#include "Camera.h"

namespace pbr
{
    Camera::Camera(int width, 
                   int height, 
                   float fov, 
                   float near, 
                   float far,
                   const glm::mat4& viewMatrix)
    {
        m_Width = width;
        m_Height = height;
        m_Fov = fov;
        m_Near = near;
        m_Far = far;

        m_ViewMatrix = viewMatrix;
        m_ProjectionMatrix = glm::perspective(glm::radians(fov), static_cast<float>(width) / static_cast<float>(height), 
                                              near, far);

        m_ViewProjMatrix = m_ViewMatrix * m_ProjectionMatrix;
        m_InvViewMatrix = glm::inverse(m_ViewMatrix);
        m_InvProjMatrix = glm::inverse(m_ProjectionMatrix);
        m_InvViewProjMatrix = glm::inverse(m_ViewProjMatrix);

        m_BlCornerNdc = glm::vec4(0, 0, 0, 1);
        m_TlCornerNdc = glm::vec4(0, 1, 0, 1);
        m_BrCornerNdc = glm::vec4(1, 0, 0, 1);
        m_TrCornerNdc = glm::vec4(1, 1, 0, 1);

        m_BlCornerWorld = m_InvViewProjMatrix * m_BlCornerNdc;
        m_TlCornerWorld = m_InvViewProjMatrix * m_TlCornerNdc;
        m_BrCornerWorld = m_InvViewProjMatrix * m_BrCornerNdc;
        m_TrCornerWorld = m_InvViewProjMatrix * m_TrCornerNdc;

        m_BlCornerWorld = m_BlCornerWorld / m_BlCornerWorld.w;
        m_TlCornerWorld = m_TlCornerWorld / m_TlCornerWorld.w;
        m_BrCornerWorld = m_BrCornerWorld / m_BrCornerWorld.w;
        m_TrCornerWorld = m_TrCornerWorld / m_TrCornerWorld.w;
    }
}