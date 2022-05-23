#pragma once

#include "hittable.h"

#include <vector>
#include <memory>   // shared_ptr

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CHittableList : public CHittable
{
public:
    void    Add(std::shared_ptr<CHittable> object) { m_hittables.push_back(object); };
    void    Clear() { m_hittables.clear(); };

    virtual bool    Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const override;
private:
    std::vector<std::shared_ptr<CHittable>>     m_hittables;
};

//----------------------------------------------------

bool    CHittableList::Hit(const CRay &ray, float t_min, float t_max, SHitRec &hitRec) const
{
    SHitRec hitTmp;
    bool    isHit = false;
    float   tClosest  = infinity;

    for (const auto &obj : m_hittables) {
        if (obj->Hit(ray, 0, tClosest, hitTmp))
        {
            hitRec = hitTmp;
            tClosest = hitTmp.t;
            isHit = true;
        }
    }

    return isHit;
}

//----------------------------------------------------
_CR_NAMESPACE_END
