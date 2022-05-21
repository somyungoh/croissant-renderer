#include "hittable.h"


_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CSphere::CSphere(const glm::vec3 &origin, float radius)
: m_origin(origin)
, m_radius(radius)
{
}

//----------------------------------------------------

bool    CSphere::Hit(const CRay &ray, SHitRec &hitRec) const
{
    glm::vec3   oc = ray.m_origin - m_origin;
    float       b = glm::dot(oc, ray.m_dir);
    float       c = glm::dot(oc, oc) - m_radius * m_radius;
    float       h = b * b - c;

    if (h < 0.0)
        return false;

    hitRec.t = -b - glm::sqrt(h);
    hitRec.p = ray.At(hitRec.t);
    hitRec.n = (hitRec.p - m_origin) / m_radius;
    hitRec.frontFace = glm::dot(hitRec.n, ray.m_dir) < 0;

    return true;
}

//----------------------------------------------------
_CR_NAMESPACE_END
