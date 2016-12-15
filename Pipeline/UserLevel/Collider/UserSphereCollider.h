#pragma once

#include "UserLevel/Collider/UserCollider.h"

namespace Pipeline
{
    class UserSphereCollider : public UserCollider
    {
    public:
        UserSphereCollider();

        void SetRadius(float radius);
        float GetRadius() const;

    private:
        float m_Radius;
    };
}
