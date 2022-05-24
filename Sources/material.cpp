#include "hittable.h"
#include "material.h"

#include "glm/gtc/random.hpp"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CLambertian::CLambertian(const glm::vec3 &color)
: m_albedo(color)
{
};

//----------------------------------------------------

bool    CLambertian::Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const
{
    cr::CRay    diffuseRay = cr::CRay(hitRec.p, hitRec.n + glm::vec3(glm::sphericalRand(1.0)));
    attenuation = m_albedo;
    scattered = diffuseRay;

    // corner case: random generated vector has same direction to the normal
    if (diffuseRay.m_dir.length() < _EPSILON)
        diffuseRay.m_dir = hitRec.n;

    return true;
}

//----------------------------------------------------
_CR_NAMESPACE_END
