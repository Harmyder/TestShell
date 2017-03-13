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


        const auto& GetTransform() const { return transform_; }
        void SetTransform(const DirectX::XMFLOAT4X3& transform) { transform_ = transform; }

        const MeshGeometry &GetGeometry() const { return meshGeometry_; }
        MeshGeometry &GetGeometryNonConst() { return meshGeometry_; }
        uint_t GetRenderItemIndex() const { return renderItemIndex_; }
        void SetRenderItemIndex(uint_t index) { renderItemIndex_ = index; }

        const InputMesh& GetInput() const { return input_; }

    private:
        const InputMesh &input_;
        uint_t renderItemIndex_;

        MeshGeometry meshGeometry_;

        DirectX::XMFLOAT4X3 transform_;
    };
}
