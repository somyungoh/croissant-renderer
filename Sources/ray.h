#pragma once

#include "cr_include.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CRay
{
public:
    CRay(const glm::vec3 &origin, const glm::vec3 &dir)
    : m_origin(origin)
    , m_dir(dir)
    {
    }

    glm::vec3   At(float t) const { return m_origin + m_dir * t; }

public:
    glm::vec3   m_origin;
    glm::vec3   m_dir;
};

//----------------------------------------------------
_CR_NAMESPACE_END
