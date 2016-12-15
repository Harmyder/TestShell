#pragma once

#include "UserLevel/Collider/UserCollider.h"

namespace Pipeline
{
    class UserBoxCollider : public UserCollider
    {
    public:
        UserBoxCollider();

        void SetHalfExtents(const XMFLOAT3& halfExtents);
        const XMFLOAT3 &GetHalfExtents() const;

    private:
        XMFLOAT3 m_HalfExtents;
    };
}