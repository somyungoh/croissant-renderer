#include "hittable.h"


_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CSphere::CSphere(const glm::vec3 &origin, float radius)
: m_origin(origin)
, m_radius(radius)
{
}

//----------------------------------------------------

bool    CSphere::Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const
{
    glm::vec3   oc = ray.m_origin - m_origin;
    float       b = glm::dot(oc, ray.m_dir);
    float       c = glm::dot(oc, oc) - m_radius * m_radius;
    float       h = b * b - c;

    if (h < 0.0)
        return false;

    // TODO: handle t for back face hit
    hitRec.t = -b - glm::sqrt(h);   // also, -b + glm::sqrt(h)
    if (hitRec.t < t_min || hitRec.t > t_max)
        return false;

    hitRec.p = ray.At(hitRec.t);
    hitRec.n = (hitRec.p - m_origin) / m_radius;
    hitRec.setFaceNormal(ray);

    return true;
}

//----------------------------------------------------
_CR_NAMESPACE_END
