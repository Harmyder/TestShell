#pragma once

#include "Pile/Attribute/NonCopyable.h"

#include <DirectXMath.h>

namespace Pipeline
{
    class InputMesh;

    struct MeshGeometry
    {
        struct Vertex
        {
            DirectX::XMFLOAT3 Position;
            DirectX::XMFLOAT4 Color;
        };

        std::vector<Vertex> Vertices;
        std::vector<uint16> Indices;
    };

    class UserMesh : public Pile::NonCopyable
    {
    public:
        UserMesh(const InputMesh &mesh);

        void SetName(const char *name);
        const char *GetName();

        const DirectX::XMFLOAT4X4& GetTransform() const;
        void SetTransform(const DirectX::XMFLOAT4X4& transform);

        const MeshGeometry &GetGeometry() const;
        MeshGeometry &GetNonConstGeometry();

        const InputMesh& GetInput() const;

    private:
        const InputMesh &m_Input;

        const char *name_;
        MeshGeometry meshGeometry_;

        DirectX::XMFLOAT4X4 m_Transform;
    };
}
