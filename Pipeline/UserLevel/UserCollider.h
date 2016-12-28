#pragma once

#include "Pile\Attribute\NonCopyable.h"
#include "Pile\Attribute\NamedObject.h"
#include "Pipeline\ColliderType.h"

#include <DirectXMath.h>

namespace Pipeline
{
    class InputCollider;

    class UserCollider : public Pile::NonCopyable, public Pile::NamedByRefObject
    {
    public:
        UserCollider(ColliderType type, const InputCollider& input);

        ColliderType GetType() const { return type_; }

        const DirectX::XMFLOAT4X4& GetTransform() const { return transform_; }
        void SetTransform(const DirectX::XMFLOAT4X4& transform) { transform_ = transform; }

        virtual DirectX::XMFLOAT4X4 GetScalingTransform() const = 0;

    private:
        const InputCollider &input_;

        const ColliderType type_;
        DirectX::XMFLOAT4X4 transform_;
    };

    class UserColliderBox : public UserCollider
    {
    public:
        UserColliderBox(const InputCollider& input, const DirectX::XMFLOAT3& extents) :
            UserCollider(ColliderType::kBox, input),
            extents_(extents)
        {}

        const DirectX::XMFLOAT3 &GetExtents() const { return extents_; }
        DirectX::XMFLOAT4X4 GetScalingTransform() const override;

    private:
        DirectX::XMFLOAT3 extents_;
    };

    class UserColliderSphere : public UserCollider
    {
    public:
        UserColliderSphere(const InputCollider& input, float radius) :
            UserCollider(ColliderType::kSphere, input),
            radius_(radius)
        {}

        float GetRadius() const { return radius_; }
        DirectX::XMFLOAT4X4 GetScalingTransform() const override;

    private:
        float radius_;
    };
}
