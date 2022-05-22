#pragma once

#include "common.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

struct SHitRec
{
    glm::vec3   p;
    glm::vec3   n;
    float       t;
    bool        frontFace;
};

//----------------------------------------------------

class CHittable
{
public:
    virtual bool    Hit(const CRay &ray, SHitRec &hitRec) const = 0;
};

//----------------------------------------------------

class CSphere : public CHittable
{
public:
    CSphere(const glm::vec3 &origin, float radius);

    virtual bool    Hit(const CRay &ray, SHitRec &hitRec) const override;

public:
    glm::vec3   m_origin;
    float       m_radius;
};

//----------------------------------------------------
_CR_NAMESPACE_END
