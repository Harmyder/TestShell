#pragma once

#include "Common\Attribute\NamedObject.h"
#include "Common\Attribute\NonCopyable.h"
#include "ColliderType.h"

namespace Pipeline {
    class InputCollider : public Common::NamedByCopyObject, public Common::NonCopyable
    {
    public:
        InputCollider(const std::string& name, const XMFLOAT4X4& transform, ColliderType type) : NamedByCopyObject(name), 
            type_(type),
            transform_(transform)
        {}
        virtual ~InputCollider() {}

        ColliderType GetType() const { return type_; }
        const XMFLOAT4X4& GetTransform() const { return transform_; }

    private:
        const ColliderType type_;
        XMFLOAT4X4 transform_;
    };

    class InputBoxCollider : public InputCollider {
    public:
        InputBoxCollider(const std::string& name, const XMFLOAT4X4& transform, float x, float y, float z) :
            InputCollider(name, transform, ColliderType::kBox),
            extents_(x, y, z)
        {}

        XMFLOAT3 GetExtents() const { return extents_; }

    private:
        const XMFLOAT3 extents_;
    };

    class InputSphereCollider : public InputCollider {
    public:
        InputSphereCollider(const std::string& name, const XMFLOAT4X4& transform, float radius) :
            InputCollider(name, transform, ColliderType::kSphere),
            radius_(radius)
        {}

        float GetRadius() const { return radius_; }

    private:
        const float radius_;
    };

    class InputCapsuleCollider : public InputCollider {
    public:
        InputCapsuleCollider(const std::string& name, const XMFLOAT4X4& transform, float height, float radius) :
            InputCollider(name, transform, ColliderType::kCapsule),
            height_(height), radius_(radius)
        {}

    private:
        const float height_, radius_;
    };
}
