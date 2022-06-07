#pragma once

#include "common.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CMaterial;

struct SHitRec
{
    glm::vec3   p;
    glm::vec3   n;
    float       t;
    std::shared_ptr<CMaterial>  p_material;
    bool        frontFace;

    void        setFaceNormal(const CRay &ray)
    {
        frontFace = glm::dot(ray.m_dir, n) < 0;
        n = frontFace ? n : -n;
    }
};

//----------------------------------------------------

class IHittable
{
public:
    virtual bool    Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const = 0;
};

//----------------------------------------------------

class CHittableSphere : public IHittable
{
public:
    CHittableSphere(const glm::vec3 &origin, float radius, const std::shared_ptr<CMaterial> &material);

    virtual bool    Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const override;

public:
    glm::vec3                   m_origin;
    float                       m_radius;
    std::shared_ptr<CMaterial>  m_material;
};


//----------------------------------------------------

class CHittableMesh : public IHittable
{
public:
    CHittableMesh(const glm::vec3 &origin, const std::shared_ptr<CMaterial> &material);

    virtual bool    Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const override;
    bool            Load(const char* file);

public:
    glm::vec3                   m_origin;
    std::shared_ptr<CMaterial>  m_material;

private:
    // mesh data
    std::vector<glm::vec3>      m_vertices;
    std::vector<uint32_t>       m_indices;

    // trianlge data (precompute purpose)
    std::vector<glm::vec3>      m_edges;
    std::vector<glm::vec3>      m_normals;

    bool                        m_isMeshLoaded;
};

//----------------------------------------------------
_CR_NAMESPACE_END
