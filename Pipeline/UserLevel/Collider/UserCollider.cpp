#include "stdafx.h"

#include "UserLevel/Collider/UserCollider.h"

namespace Pipeline
{
    UserCollider::UserCollider(Type type) :
        m_Type(type)
    {
    }

    template <class Concrete>
    Concrete *UserCollider::CastAs()
    {
        return static_cast<Concrete*>(this);
    }

    UserCollider::Type UserCollider::GetType() const
    {
        return m_Type;
    }

    void UserCollider::SetName(const char *name)
    {
        name_ = name;
    }

    const char *UserCollider::GetName() const
    {
        return name_;
    }

    const XMFLOAT4X4& UserCollider::GetTransform() const
    {
        return m_Transform;
    }

    void UserCollider::SetTransform(const XMFLOAT4X4& transform)
    {
        m_Transform = transform;
    }
}
