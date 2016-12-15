#pragma once

#include "Pile/Attribute/NonCopyable.h"

#include <DirectXMath.h>

namespace Pipeline
{
    class UserCollider : public Pile::NonCopyable
    {
    public:
        enum Type
        {
            BOX,
            SPHERE,
        };

    protected:
        UserCollider(Type type);

    public:
        template <class Concrete>
        Concrete *CastAs();

        Type GetType() const;

        void SetName(const char *name);
        const char *GetName() const;

        const DirectX::XMFLOAT4X4& GetTransform() const;
        void SetTransform(const DirectX::XMFLOAT4X4& transform);

    private:
        const Type m_Type;
        const char *name_;
        DirectX::XMFLOAT4X4 m_Transform;
    };
}
