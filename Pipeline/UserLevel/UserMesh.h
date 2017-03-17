#pragma once

#include "Common/Attribute/NonCopyable.h"
#include "Common/Attribute/NamedObject.h"
#include "Common/Math/Matrix.h"
#include "Viewer/Vertex.h"

namespace Pipeline
{
    class InputMesh;

    struct MeshGeometry
    {
        std::vector<Common::Vector3> UniquePositions;
        std::vector<uint16>          TrianglesPositions;
        std::vector<Viewer::VertexNormalTex> UniqueVertices;
        std::vector<uint16>                  TrianglesVertices;
    };

    class UserMesh : public Common::NamedByRefObject, public Common::NonCopyable
    {
    public:
        UserMesh(const InputMesh &mesh);


        const auto& GetTransform() const { return transform_; }
        void SetTransform(const Common::Matrix4& transform) { transform_ = transform; }

        const MeshGeometry &GetGeometry() const { return meshGeometry_; }
        MeshGeometry &GetGeometryNonConst() { return meshGeometry_; }
        uint_t GetRenderItemIndex() const { return renderItemIndex_; }
        void SetRenderItemIndex(uint_t index) { renderItemIndex_ = index; }

        const InputMesh& GetInput() const { return input_; }

    private:
        const InputMesh &input_;
        uint_t renderItemIndex_;

        MeshGeometry meshGeometry_;

        Common::Matrix4 transform_;
    };
}
