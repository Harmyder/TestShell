#include "stdafx.h"

#include "UserLevel/Collider/UserSphereCollider.h"

namespace Pipeline
{
    UserSphereCollider::UserSphereCollider() :
        UserCollider(UserCollider::SPHERE)
    {}

    void UserSphereCollider::SetRadius(float radius)
    {
        m_Radius = radius;
    }

    float UserSphereCollider::GetRadius() const
    {
        return m_Radius;
    }
}
