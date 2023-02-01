#pragma once

#include "common.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class ITexture
{
public:
    virtual glm::vec3   Eval(float u, float v, const glm::vec3& p) const = 0;
};

//----------------------------------------------------

class CTextureConstant : public ITexture
{
public:
    CTextureConstant() {}
    CTextureConstant(const glm::vec3& c) : m_color(c) {};
    virtual glm::vec3   Eval(float u, float v, const glm::vec3& p) const override { return m_color; }

public:
    glm::vec3 m_color;
};

//----------------------------------------------------

class CTextureChecker : public ITexture
{
public:
    CTextureChecker() {}
    CTextureChecker(const glm::vec3& c1, const glm::vec3& c2) : m_colorEven(c1), m_colorOdd(c2) {};
    virtual glm::vec3   Eval(float u, float v, const glm::vec3& p) const override { 
        float sin = glm::sin(10 * p.x) * glm::sin(10 * p.y) * glm::sin(10 * p.z);
        if (sin > 0)
            return m_colorEven;
        return m_colorOdd;
    }

public:
    glm::vec3 m_colorEven;
    glm::vec3 m_colorOdd;
};

//----------------------------------------------------
_CR_NAMESPACE_END
