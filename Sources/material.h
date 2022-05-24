#pragma once

#include "common.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

struct SHitRec;

class CMaterial
{
public:
    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const = 0;
};

//----------------------------------------------------

class CLambertian : public CMaterial
{
public:
    CLambertian(const glm::vec3 &color);

    virtual bool    Scatter(const CRay &ray, const SHitRec &hitRec, glm::vec3 &attenuation, CRay &scattered) const override;

public:
    glm::vec3   m_albedo;
};

//----------------------------------------------------
_CR_NAMESPACE_END
