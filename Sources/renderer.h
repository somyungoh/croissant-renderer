#pragma once

#include "common.h"
#include "ray.h"

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CHittableList;
class CCamera;

//----------------------------------------------------

struct SRenderSetting
{
    u_int32_t   render_w, render_h;
    u_int32_t   nSamples;
    u_int32_t   nMaxDepth;

    // AA
    u_int32_t   nSamplesW;
    float       nSamplesOffset;
};

//----------------------------------------------------

class CRenderer
{
public:
    CRenderer();

    void    FullRender();
    void    ProgressiveRender();
    void    GetLastRender(float* &outMap);

    void    SetRenderSetting(const SRenderSetting &renderSetting);
    void    InitScene();

    bool    IsFinished() { return m_isFinished; };

private:
    glm::vec3   _RecursiveRaycast(const CRay &ray, int depth);
    void        _ClearOldRender();

private:
    std::shared_ptr<CHittableList>  m_scene;
    std::shared_ptr<CCamera>        m_camera;

    SRenderSetting                  m_renderSetting;
    bool                            m_isFinished;
    u_int32_t                       m_currentSample;    // for progressive rendering

    float*                          m_pixmap;
};

//----------------------------------------------------
_CR_NAMESPACE_END
