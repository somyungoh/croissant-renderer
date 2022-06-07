#include "hittable.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CHittableSphere::CHittableSphere(const glm::vec3 &origin, float radius, const std::shared_ptr<CMaterial> &material)
: m_origin(origin)
, m_radius(radius)
, m_material(material)
{
}

//----------------------------------------------------

bool    CHittableSphere::Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const
{
    glm::vec3   oc = ray.m_origin - m_origin;
    float       b = glm::dot(oc, ray.m_dir);
    float       c = glm::dot(oc, oc) - m_radius * m_radius;
    float       h = b * b - c;

    if (h < 0.0)
        return false;

    hitRec.t = -b - glm::sqrt(h);
    if (hitRec.t < t_min)
        hitRec.t = -b + glm::sqrt(h);
    if (hitRec.t < t_min || hitRec.t > t_max)
        return false;

    hitRec.p = ray.At(hitRec.t);
    hitRec.n = (hitRec.p - m_origin) / m_radius;
    hitRec.setFaceNormal(ray);
    hitRec.p_material = m_material;

    return true;
}

//----------------------------------------------------

CHittableMesh::CHittableMesh(const glm::vec3 &origin, const std::shared_ptr<CMaterial> &material)
: m_origin(origin)
, m_material(material)
, m_isMeshLoaded(false)
{
}

//----------------------------------------------------

bool    CHittableMesh::Load(const char* file)
{
    // load obj
    tinyobj::attrib_t                   attrib;
    std::vector<tinyobj::shape_t>       shapes;
    std::vector<tinyobj::material_t>    materials;

    std::string warn;
    std::string err;

    printf("[Mesh] Loading obj - %s\n", file);
    bool res = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err,
                                file, NULL, true);

    if (!warn.empty())
        printf("[Mesh] Warn: %s\n", warn.c_str());

    if (!err.empty())
        printf("[Mesh] Err: %s\n", err.c_str());
    
    if (!res)
    {
        printf("[Mesh] Failed to load obj - %s\n", file);
        return false;
    }

    // TODO: Support multi-shape obj
    // We assume that the object structure has a single shape
    if (shapes.size() != 1)
        throw std::runtime_error("More than one shape found in the mesh!");

    printf("[Mesh] # of vertices  : %lu\n", attrib.vertices.size() / 3);
    printf("[Mesh] # of normals   : %lu\n", attrib.normals.size() / 3);
    printf("[Mesh] # of faces     : %lu\n", shapes[0].mesh.indices.size() / 3);

    std::vector<glm::vec3> uniqueVertices;
    std::vector<uint32_t> uniqueIndices;

    // copy vertices and indices
    for (const auto& shape : shapes) {
        // tiny obj loader did a triangulation for us, so we directly load them
        // into our member, with offset of 3
        for (const auto& index : shape.mesh.indices)
        {
            // vertex
            glm::vec3 v(
                static_cast<float>(attrib.vertices[3 * index.vertex_index + 0]),
                static_cast<float>(attrib.vertices[3 * index.vertex_index + 1]),
                static_cast<float>(attrib.vertices[3 * index.vertex_index + 2])
            );

            auto it = std::find(uniqueVertices.begin(), uniqueVertices.end(), v);
            size_t indicesIdx;
            // only keep unique vertices
            if (it == uniqueVertices.end())
            {
                uniqueVertices.push_back(v);
                uniqueIndices.push_back(static_cast<uint32_t>(m_vertices.size()));
                m_vertices.push_back(v);

                indicesIdx = uniqueVertices.size() - 1;
            }
            else
                indicesIdx = it - uniqueVertices.begin();

            // indices
            m_indices.push_back(uniqueIndices[indicesIdx]);
        }
    }

    // precompute triangle data
    m_edges.resize(m_indices.size());
    m_normals.resize(m_indices.size() / 3);
    for (size_t i = 0; i < m_indices.size() / 3; i++)
    {
        // edges
        glm::vec3 e0 = m_vertices[m_indices[i * 3 + 1]] - m_vertices[m_indices[i * 3 + 0]];
        glm::vec3 e1 = m_vertices[m_indices[i * 3 + 2]] - m_vertices[m_indices[i * 3 + 1]];
        glm::vec3 e2 = m_vertices[m_indices[i * 3 + 0]] - m_vertices[m_indices[i * 3 + 2]];
        glm::vec3 n  = glm::cross(e0, e2);
        m_edges[i * 3 + 0] = e0;
        m_edges[i * 3 + 1] = e1;
        m_edges[i * 3 + 2] = e2;
        // normal
        m_normals[i] = n;
    }

    printf("[Mesh] Done.\n");

    m_isMeshLoaded = true;

    return true;
}

//----------------------------------------------------

bool    CHittableMesh::Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const
{
    if (!m_isMeshLoaded)
    {
        std::cerr << "Error: Attempting to use mesh without loading!\n";
        return false;
    }

    bool    hasHit = false;
    float   tClosest = _INFINITY;

    // Loop for all triangles
    for (size_t i = 0; i < m_indices.size() / 3; i++)
    {
        uint32_t    i0 = m_indices[i * 3 + 0];
        uint32_t    i1 = m_indices[i * 3 + 1];
        uint32_t    i2 = m_indices[i * 3 + 2];
        glm::vec3   v0 = m_vertices[i0];
        glm::vec3   v1 = m_vertices[i1];
        glm::vec3   v2 = m_vertices[i2];
        glm::vec3   v1v0 = v1 - v0;
        glm::vec3   v2v0 = v2 - v0;
        glm::vec3   rov0 = ray.m_origin - v0;
        glm::vec3   n = glm::cross( v1v0, v2v0 );
        glm::vec3   q = glm::cross( rov0, ray.m_dir );
        float       d = 1.0f / dot( ray.m_dir, n );
        float       u = d * glm::dot( -q, v2v0 );
        float       v = d * glm::dot(  q, v1v0 );
        float       t = d * glm::dot( -n, rov0 );

        if (u < 0.0f || v < 0.0f || (u + v) > 1.0f)
            continue;
        else if (t < t_min || t > t_max)
            return false;

        // there is a hit
        if (t < tClosest)
        {
            hitRec.t = t;
            hitRec.p = ray.At(t);
            hitRec.n = m_normals[i0 / 3];
            hitRec.setFaceNormal(ray);
            hitRec.p_material = m_material;
            hasHit = true;
        }
    }

    return hasHit;
}

//----------------------------------------------------

_CR_NAMESPACE_END
