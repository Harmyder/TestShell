#pragma once

#include "Pile/Attribute/NonCopyable.h"

#include "Viewer\Vertex.h"

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
        UserMesh(const InputMesh &mesh);

        void SetName(const char *name);
        const char *GetName();

        const DirectX::XMFLOAT4X4& GetTransform() const;
        void SetTransform(const DirectX::XMFLOAT4X4& transform);

        const MeshGeometry &GetGeometry() const;
        MeshGeometry &GetNonConstGeometry();
        uint_t GetRenderItemIndex() const { return renderItemIndex_; }
        void SetRenderItemIndex(uint_t index) { renderItemIndex_ = index; }

        const InputMesh& GetInput() const;

    private:
        const InputMesh &input_;
        uint_t renderItemIndex_;

        const char *name_;
        MeshGeometry meshGeometry_;

        DirectX::XMFLOAT4X4 transform_;
    };
}
