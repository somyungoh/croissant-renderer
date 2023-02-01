#include "hittable.h"
#include "material.h"

#include "glm/gtc/random.hpp"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CMaterialLambertian::CMaterialLambertian(const glm::vec3& color)
: m_albedo(std::make_unique<CTextureConstant>(color))
{
}

//----------------------------------------------------

CMaterialLambertian::CMaterialLambertian(std::unique_ptr<ITexture>& texture)
: m_albedo(std::move(texture))
{
}

//----------------------------------------------------

bool    CMaterialLambertian::Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const
{
    cr::CRay    diffuseRay = cr::CRay(hitRec.p, hitRec.n + glm::vec3(glm::sphericalRand(1.0)));
    attenuation = m_albedo->Eval(0, 0, hitRec.p);
    scattered = diffuseRay;

    // corner case: random generated vector has same direction to the normal
    if (diffuseRay.m_dir.length() < _EPSILON)
        diffuseRay.m_dir = hitRec.n;

    return true;
}

//----------------------------------------------------

CMaterialMetal::CMaterialMetal(const glm::vec3 &color, float glossiness)
: m_albedo(color)
, m_glossiness(glossiness < 1.0 ? glossiness : 1.0)
{
}

//----------------------------------------------------

bool    CMaterialMetal::Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const
{
    glm::vec3   reflectedDir = glm::reflect(ray.m_dir, hitRec.n);
    cr::CRay    reflectedRay = cr::CRay(hitRec.p, reflectedDir + m_glossiness * glm::vec3(glm::sphericalRand(1.0f)));
    attenuation = m_albedo;
    scattered = reflectedRay;

    return (glm::dot(reflectedRay.m_dir, hitRec.n) > 0);
}

//----------------------------------------------------

CMaterialGlass::CMaterialGlass(float refractiveIndex, float glossiness)
: m_refractiveIndex(refractiveIndex)
, m_glossiness(glossiness < 1.0 ? glossiness : 1.0)
{
}

//----------------------------------------------------

bool    CMaterialGlass::Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const
{
    float       refractiveRatio = hitRec.frontFace ? (1.0f / m_refractiveIndex) : m_refractiveIndex;
    float       cosTheta = fmin(glm::dot(-ray.m_dir, hitRec.n), 1.0);
    float       sinTheta = glm::sqrt(1.0 - cosTheta * cosTheta);

    bool        canRefract = (refractiveRatio * sinTheta <= 1.0);
    glm::vec3   outDir;

    if (canRefract || (_Reflectance(cosTheta, refractiveRatio) > (float)rand() / RAND_MAX))
        outDir = glm::refract(ray.m_dir, hitRec.n, refractiveRatio);
    else    // reflect
        outDir =  glm::reflect(ray.m_dir, hitRec.n);

    attenuation = glm::vec3(1.f);
    scattered = CRay(hitRec.p, outDir);

    return true;
}

//----------------------------------------------------

float   CMaterialGlass::_Reflectance(float cos, float refIdx) const
{
    // Use Schlick's approximation for reflectance.
    auto r0 = (1.0f -refIdx) / (1.0f + refIdx);
    r0 = r0*r0;

    return r0 + (1.0 - r0) * glm::pow((1.0f - cos), 5.0f);
}

//----------------------------------------------------
_CR_NAMESPACE_END
