#include "hittable_list.h"


_CR_NAMESPACE_BEGIN
//----------------------------------------------------

void    CHittableList::Add(std::shared_ptr<CHittable> object)
{
    m_hittables.push_back(object);
}

//----------------------------------------------------

void    CHittableList::Clear()
{
    m_hittables.clear();
}

//----------------------------------------------------

bool    CHittableList::Hit(const CRay &ray, SHitRec &hitRec) const
{
    SHitRec hitTmp;
    bool    isHit = false;
    float   tmin  = 10000;

    for (const auto &obj : m_hittables) {
        if (obj->Hit(ray, hitTmp)) {
            if (hitTmp.t < tmin)
            {
                tmin = hitTmp.t;
                hitRec = hitTmp;
                isHit = true;
            }
        }
    }

    return isHit;
}

//----------------------------------------------------
_CR_NAMESPACE_END
