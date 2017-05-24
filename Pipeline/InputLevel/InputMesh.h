#pragma once

#include "Common/Attribute/NamedObject.h"
#include "Common/Geometry/GeometryGenerator.h"
#include "Common/Math/Vector/all.h"

namespace Pipeline
{
    class InputMesh : public Common::NamedByCopyObject
    {
    public:
        InputMesh(const std::string& name) : NamedByCopyObject(name) {}

        void SetPositions(std::vector<XMFLOAT3> &positions) { positions_.swap(positions); }
        void SetTrianglesPositions(std::vector<uint16> &triangles) { trianglesPositions_.swap(triangles); }
        void SetNormals(std::vector<XMFLOAT3> &normals) { normals_.swap(normals); }
        void SetTexCoords(std::vector<XMFLOAT2> &texCoords) { texCoords_.swap(texCoords); }
        void SetTrianglesTexCoords(std::vector<uint16> &triangles) { trianglesTexCoords_.swap(triangles); }

        uint_t GetVerticesCount() const { return positions_.size(); }
        const std::vector<XMFLOAT3>& GetPositions() const { return positions_; }
        const std::vector<uint16>&   GetTrianglesPositions() const { return trianglesPositions_; }
        const std::vector<XMFLOAT3>& GetNormals() const { return normals_; }
        const std::vector<XMFLOAT2>& GetTexCoords() const { return texCoords_; }
        const std::vector<uint16>&   GetTrianglesTexCoords() const { return trianglesTexCoords_; }

        void SetTransform(const Common::OrthogonalTransform& transform);
        void SetTransforms(std::vector<XMFLOAT4X3>&& transform);

        const Common::OrthogonalTransform& GetTransform() const { assert(isInstanced_ == InstanceOption::NotInstanced); return transform_; }
        const std::vector<XMFLOAT4X3>& GetTransforms() const { assert(isInstanced_ == InstanceOption::Instanced); return transforms_; }

        using Vertex = Common::GeometryGenerator::Vertex;
        void SetVisualVertices(std::vector<Vertex> uniqueVertices) { uniqueVertices_ = uniqueVertices; }
        void SetTrianglesVertices(std::vector<uint16> trianglesVertices) { trianglesVertices_ = trianglesVertices; }
        const std::vector<Vertex>& GetVisualVertices() const { return uniqueVertices_; } // unique triplets
        const std::vector<uint16>& GetTrianglesVertices() const { return trianglesVertices_; } // per triangle

        enum class InstanceOption {
            None = 1,
            NotInstanced = 2,
            Instanced = 4,
            CanBeNotInstanced = None | NotInstanced,
            CanBeInstanced = None | Instanced,
        };

    private:
        std::vector<XMFLOAT3> positions_;
        std::vector<uint16>   trianglesPositions_;
        std::vector<XMFLOAT3> normals_;
        std::vector<XMFLOAT2> texCoords_;
        std::vector<uint16>   trianglesTexCoords_;

        Common::OrthogonalTransform transform_;
        InstanceOption isInstanced_ = InstanceOption::None;
        std::vector<XMFLOAT4X3> transforms_;

        std::vector<Vertex> uniqueVertices_;
        std::vector<uint16> trianglesVertices_;
    };
}
