#pragma once

#include "Common\Attribute\NonCopyable.h"
#include "Common\Attribute\NamedObject.h"

#include "Viewer\Vertex.h"

namespace Pipeline
{
    class InputMesh;

    struct MeshGeometry
    {
        std::vector<Viewer::VertexNormalTex> Vertices;
    };

    class UserMesh : public Common::NamedByRefObject, public Common::NonCopyable
    {
    public:
        UserMesh(const InputMesh &mesh);

        const DirectX::XMFLOAT4X4& GetTransform() const;
        void SetTransform(const DirectX::XMFLOAT4X4& transform);

        const MeshGeometry &GetGeometry() const;
        MeshGeometry &GetGeometryNonConst();
        uint_t GetRenderItemIndex() const { return renderItemIndex_; }
        void SetRenderItemIndex(uint_t index) { renderItemIndex_ = index; }

        const InputMesh& GetInput() const;

    private:
        const InputMesh &input_;
        uint_t renderItemIndex_;

        MeshGeometry meshGeometry_;

        DirectX::XMFLOAT4X4 transform_;
    };
}
