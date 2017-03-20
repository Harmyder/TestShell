#pragma once

#include "Common/Attribute/NamedObject.h"
#include "Common/Geometry/GeometryGenerator.h"

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

        void SetTransform(const XMFLOAT4X4& transform) { transform_ = transform; }
        const XMFLOAT4X4& GetTransform() const { return transform_; }

        using Vertex = Common::GeometryGenerator::Vertex;
        void SetVisualVertices(std::vector<Vertex> uniqueVertices) { uniqueVertices_ = uniqueVertices; }
        void SetTrianglesVertices(std::vector<uint16> trianglesVertices) { trianglesVertices_ = trianglesVertices; }
        const std::vector<Vertex>& GetVisualVertices() const { return uniqueVertices_; } // unique triplets
        const std::vector<uint16>& GetTrianglesVertices() const { return trianglesVertices_; } // per triangle

    private:
        std::vector<XMFLOAT3> positions_;
        std::vector<uint16>   trianglesPositions_;
        std::vector<XMFLOAT3> normals_;
        std::vector<XMFLOAT2> texCoords_;
        std::vector<uint16>   trianglesTexCoords_;

        XMFLOAT4X4 transform_;

        std::vector<Vertex> uniqueVertices_;
        std::vector<uint16> trianglesVertices_;
    };
}
