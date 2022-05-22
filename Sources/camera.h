#pragma once

#include "common.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CCamera
{
public:
    CCamera (const glm::vec3 &origin, const glm::vec3 &up, const glm::vec3 &forward, float aspectRatio, float focalDist = 1.0f)
    : m_origin(origin)
    , m_up(up)
    , m_forward(forward)
    , m_aspectRatio(aspectRatio)
    , m_focalDist(focalDist)
    {
        m_right = glm::vec3(glm::normalize(glm::cross(up, forward)));
        m_lowerLeftCorner = m_origin - m_right * 0.5f - m_up / aspectRatio * 0.5f + m_forward * focalDist;
    }

    const CRay GetRay(float u, float v)
    {
        // TODO: check u,v out of range?
        return CRay(m_origin, m_lowerLeftCorner + u * m_right + v * m_up / m_aspectRatio - m_origin);
    }

public:
    float       m_focalDist;
    float       m_aspectRatio;
    glm::vec3   m_lowerLeftCorner;

    glm::vec3   m_origin;
    glm::vec3   m_up;
    glm::vec3   m_forward;
    glm::vec3   m_right;
};

//----------------------------------------------------
_CR_NAMESPACE_END
