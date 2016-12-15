#include "stdafx.h"

#include "UserLevel/Collider/UserBoxCollider.h"

namespace Pipeline
{
    UserBoxCollider::UserBoxCollider() :
        UserCollider(UserCollider::BOX)
    {}

    void UserBoxCollider::SetHalfExtents(const XMFLOAT3& halfExtents)
    {
        m_HalfExtents = halfExtents;
    }

    const XMFLOAT3 &UserBoxCollider::GetHalfExtents() const
    {
        return m_HalfExtents;
    }
}
