#pragma once

#include "cr_include.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CSphere
{
public:
    CSphere(const glm::vec3 &origin, float radius)
    : m_origin(origin), m_radius(radius)
    {
    }

    bool    Intersection(const CRay &ray)
    {
        glm::vec3   oc = ray.m_origin - m_origin;
        float       b = glm::dot(oc, ray.m_dir);
        float       c = glm::dot(oc, oc) - m_radius * m_radius;
        float       h = b * b - c;

        if (h < 0.0)
            return false;
        return true;
    }

public:
    glm::vec3   m_origin;
    float       m_radius;
};

//----------------------------------------------------
_CR_NAMESPACE_END
