#pragma once

#include "Pile\Attribute\NamedObject.h"

namespace Pipeline
{
    class InputMesh : public Pile::NamedByCopyObject
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

        struct Vertex
        {
            uint16 PositionIndex;
            uint16 NormalIndex;
            uint16 TexCoordIndex;

            bool operator==(const Vertex& other);
        };

        const std::vector<Vertex>& GetVertices() const;
        const std::vector<uint16>& GetTrianglesVertices() const;

    public:
        void ComputeVertices();

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
