#include "stdafx.h"

#include "UserLevel\UserCollider.h"
#include "InputLevel\InputCollider.h"

using namespace DirectX;

namespace Pipeline
{
    UserCollider::UserCollider(ColliderType type, const InputCollider& input) : NamedByRefObject(input.GetName()),
        input_(input),
        type_(type)
    {}

    XMFLOAT4X4 UserColliderBox::GetScalingTransform() const {
        auto scaling = XMMatrixScalingFromVector(XMLoadFloat3(&GetExtents()));
        XMFLOAT4X4 ret;
        XMStoreFloat4x4(&ret, scaling);
        return ret;
    }

    XMFLOAT4X4 UserColliderSphere::GetScalingTransform() const {
        auto scaling = XMMatrixScaling(GetRadius(), GetRadius(), GetRadius());
        XMFLOAT4X4 ret;
        XMStoreFloat4x4(&ret, scaling);
        return ret;
    }
}
