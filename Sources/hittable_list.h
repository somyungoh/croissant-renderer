#pragma once

#include "hittable.h"

#include <vector>
#include <memory>   // shared_ptr

_CR_NAMESPACE_BEGIN
//----------------------------------------------------

class CHittableList : public CHittable
{
public:
    void    Add(std::shared_ptr<CHittable> object);
    void    Clear();

    virtual bool    Hit(const CRay &ray, SHitRec &hitRec) const override;
private:
    std::vector<std::shared_ptr<CHittable>>     m_hittables;
};

//----------------------------------------------------
_CR_NAMESPACE_END
