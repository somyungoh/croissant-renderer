#include "renderer.h"
#include "hittable_list.h"
#include "camera.h"
#include "material.h"

#include <chrono>   // steady_clock

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

CRenderer::CRenderer()
: m_scene(std::make_shared<CHittableList>(CHittableList()))
, m_camera(std::make_shared<CCamera>(CCamera()))
, m_isFinished(false)
, m_currentSample(0)
, m_pixmap(nullptr)
{
}

//----------------------------------------------------

void    CRenderer::FullRender()
{
    if (m_pixmap == nullptr)    // initial render
        m_pixmap = new float[m_renderSetting.render_w * m_renderSetting.render_h * 3];
    else if (m_isFinished)      // previous render exists
        _ClearOldRender();

    // Render loop
    printf("[Render] Start rendering...\n");
    fflush(stdout);

    // Timer
    auto            begin = std::chrono::steady_clock::now();

    for (size_t w = 0; w < m_renderSetting.render_w; w++) {
        for (size_t h = 0; h < m_renderSetting.render_h; h++) {
            for (size_t s = 0; s < m_renderSetting.nSamples; s++)
            {
                int     si = s % m_renderSetting.nSamplesW;
                int     sj = s / m_renderSetting.nSamplesW;
                float   u = (w + (float)si / m_renderSetting.nSamplesW + m_renderSetting.nSamplesOffset) / m_renderSetting.render_w;
                float   v = (h + (float)sj / m_renderSetting.nSamplesW + m_renderSetting.nSamplesOffset) / m_renderSetting.render_h;
                CRay        ray = m_camera->GetRay(u, v);
                glm::vec3   color = _RecursiveRaycast(ray, m_renderSetting.nMaxDepth);

                m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 0] += color.r;
                m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 1] += color.g;
                m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 2] += color.b;
            }
        }
    }

    m_isFinished = true;
    m_currentSample = m_renderSetting.nSamples;     // because it will be used for AA correction later

    // elapsed time
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
    printf("[Render] Done.\n");
    printf("[Render] Elpased: %.3fs.\n", elapsed / 1000.f);
}

//----------------------------------------------------

void    CRenderer::ProgressiveRender()
{
    if (m_pixmap == nullptr)    // initial render
        m_pixmap = new float[m_renderSetting.render_w * m_renderSetting.render_h * 3];
    else if (m_isFinished)      // previous render exists
        _ClearOldRender();

    // TODO: Timer for progressive rendering

    for (size_t w = 0; w < m_renderSetting.render_w; w++) {
        for (size_t h = 0; h < m_renderSetting.render_h; h++) {
            int     si = m_currentSample % m_renderSetting.nSamplesW;
            int     sj = m_currentSample / m_renderSetting.nSamplesW;
            float   u = (w + (float)si / m_renderSetting.nSamplesW + m_renderSetting.nSamplesOffset) / m_renderSetting.render_w;
            float   v = (h + (float)sj / m_renderSetting.nSamplesW + m_renderSetting.nSamplesOffset) / m_renderSetting.render_h;
            CRay        ray = m_camera->GetRay(u, v);
            glm::vec3   color = _RecursiveRaycast(ray, m_renderSetting.nMaxDepth);

            m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 0] += color.r;
            m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 1] += color.g;
            m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 2] += color.b;
        }
    }

    if (++m_currentSample >= m_renderSetting.nSamples)
    {
        m_isFinished = true;
        printf("[Render] Done.\n");
    }
    else
    {
        printf("[Render] Progress: %.2f%%\n", 100.f * m_currentSample / m_renderSetting.nSamples);
    }

}

//----------------------------------------------------

void    CRenderer::GetLastRender(float* &outMap)
{
    if (outMap == nullptr)
        outMap = new float[m_renderSetting.render_w * m_renderSetting.render_h * 3];

    for (size_t w = 0; w < m_renderSetting.render_w; w++) {
        for (size_t h = 0; h < m_renderSetting.render_h; h++) 
        {
            glm::vec3 rawColor( m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 0],
                                m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 1],
                                m_pixmap[(h * m_renderSetting.render_w + w) * 3 + 2]);

            // gamma correction
            float   scale = 1.0f / m_currentSample;
            rawColor = glm::sqrt(rawColor * scale);

            // copy
            outMap[(h * m_renderSetting.render_w + w) * 3 + 0] = rawColor.r;
            outMap[(h * m_renderSetting.render_w + w) * 3 + 1] = rawColor.g;
            outMap[(h * m_renderSetting.render_w + w) * 3 + 2] = rawColor.b;
        }
    }
}

//----------------------------------------------------

void    CRenderer::SetRenderSetting(const SRenderSetting &renderSetting)
{
    m_renderSetting = renderSetting;
}

//----------------------------------------------------

void    CRenderer::InitScene()
{
        // Camera
    float           aspectRatio = (float)m_renderSetting.render_w / m_renderSetting.render_h;
    m_camera = std::make_shared<CCamera>(CCamera(45.f, aspectRatio));
    m_camera->SetPos(glm::vec3(0, 0.65, -1));
    m_camera->LookAt(glm::vec3(0, 0, 0));

    // Scene
    std::shared_ptr<cr::CMaterial>  mat_labmbertGreen = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.15, 0.6, 0.09));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhite = std::make_shared<cr::CMaterialLambertian>(glm::vec3(1.0f));
    std::shared_ptr<cr::CMaterial>  mat_lambertBrown = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.92f, 0.59f, 0.17f));
    std::shared_ptr<cr::CMaterial>  mat_lambertWhiteGray = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.8f));
    std::shared_ptr<cr::CMaterial>  mat_lambertBlue = std::make_shared<cr::CMaterialLambertian>(glm::vec3(0.2, 0.18, 0.87));
    std::shared_ptr<cr::CMaterial>  mat_metalWhite = std::make_shared<cr::CMaterialMetal>(glm::vec3(1.0, 1.0, 1.0), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalGold = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.6, 0.2), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalBlue = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.2, 0.3, 0.8), 0);
    std::shared_ptr<cr::CMaterial>  mat_metalRose = std::make_shared<cr::CMaterialMetal>(glm::vec3(0.8, 0.3, 0.2), 0.2);
    std::shared_ptr<cr::CMaterial>  mat_glass = std::make_shared<cr::CMaterialGlass>(1.9, 0);

#if 1   // Use Obj
    auto croissant = std::make_shared<cr::CHittableMesh>(glm::vec3(0, 0, 0), mat_lambertBrown);
    croissant->Load("Model/Croissants_obj/Croissant.obj");
    m_scene->Add(croissant);
#else
    m_scene.Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0, 0, 0), 0.1, mat_lambertWhite)));
#endif
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.1, 0.097, 0.3), 0.15, mat_lambertWhite)));
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.35, 0.07, 0.18), 0.12, mat_metalRose)));
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(-0.3, 0.05, 0), 0.1, mat_metalWhite)));
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0.18, 0.025, -0.15), 0.05, mat_glass)));
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(-0.155, 0.06, 0.23), 0.11, mat_metalBlue)));
    m_scene->Add(std::make_shared<cr::CHittableSphere>(cr::CHittableSphere(glm::vec3(0, -10.05, 0), 10, mat_lambertWhite)));

    m_scene->BuildBVHTree();
}

//----------------------------------------------------

glm::vec3   CRenderer::_RecursiveRaycast(const CRay &ray, int depth)
{
    // max-depth reached
    if (depth <= 0) {
        return glm::vec3(0);
    }

    cr::SHitRec     hitRec;
    if (m_scene->Hit(ray, 0.00001f, _INFINITY, hitRec))
    {
        // bounced rays
        cr::CRay    scatteredRay;
        glm::vec3   attenuation;
        if (hitRec.p_material->Scatter(ray, hitRec, attenuation, scatteredRay))
            return attenuation * _RecursiveRaycast(scatteredRay, depth - 1);
        return glm::vec3(0);
    }

    // coloring
    float   t = 0.5f * (ray.m_dir.y + 1.0f);
    return glm::vec3(1.0) * (1.0f - t) + glm::vec3(0.5, 0.7, 1.0) * t;
}

//----------------------------------------------------

void    CRenderer::_ClearOldRender()
{
    m_isFinished = false;
    m_currentSample = 0;

    for (int i = 0; i < m_renderSetting.render_w * m_renderSetting.render_h * 3; ++i)
        m_pixmap[i] = 0.0f;
}

//----------------------------------------------------
_CR_NAMESPACE_END
