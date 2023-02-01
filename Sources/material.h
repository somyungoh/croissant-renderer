#pragma once

#include "common.h"
#include "ray.h"
#include "texture.h"

#include <memory>   // std::unique_ptr

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

struct SHitRec;

class IMaterial
{
public:
    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const = 0;
};

//----------------------------------------------------

class CMaterialLambertian : public IMaterial
{
public:
    CMaterialLambertian(const glm::vec3& color);
    CMaterialLambertian(std::unique_ptr<ITexture>& texture);

    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const override;

public:
    std::unique_ptr<ITexture>   m_albedo;
};

//----------------------------------------------------

class CMaterialMetal : public IMaterial
{
public:
    CMaterialMetal(const glm::vec3 &color, float glossiness);

    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const override;

public:
    glm::vec3   m_albedo;
    float       m_glossiness;
};

//----------------------------------------------------

class CMaterialGlass : public IMaterial
{
public:
    CMaterialGlass(float refrativeIndex, float glossiness);

    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const override;

public:
    float   m_refractiveIndex;
    float   m_glossiness;

private:
    float   _Reflectance(float cos, float refIdx) const;
};

//----------------------------------------------------
_CR_NAMESPACE_END
