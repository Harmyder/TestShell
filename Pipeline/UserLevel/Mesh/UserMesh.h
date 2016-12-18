#pragma once

#include "Pile/Attribute/NonCopyable.h"

#include <DirectXMath.h>

namespace Viewer {
    class RenderItem;
    struct Vertex;
}

namespace Pipeline
{
    class InputMesh;

    struct MeshGeometry
    {
        std::vector<Viewer::Vertex> Vertices;
        std::vector<uint16> Indices;
    };

    class UserMesh : public Pile::NonCopyable
    {
    public:
        UserMesh(const InputMesh &mesh, Viewer::RenderItem& renderItem);

        void SetName(const char *name);
        const char *GetName();

        const DirectX::XMFLOAT4X4& GetTransform() const;
        void SetTransform(const DirectX::XMFLOAT4X4& transform);

        const MeshGeometry &GetGeometry() const;
        MeshGeometry &GetNonConstGeometry();
        const Viewer::RenderItem &GetRenderItem() const { return renderItem_; }
        Viewer::RenderItem &GetNonConstRenderItem() { return renderItem_; }

        const InputMesh& GetInput() const;

    private:
        const InputMesh &input_;
        Viewer::RenderItem& renderItem_;

        const char *name_;
        MeshGeometry meshGeometry_;

        DirectX::XMFLOAT4X4 transform_;
    };
}
