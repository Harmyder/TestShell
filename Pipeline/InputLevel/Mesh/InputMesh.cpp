#include "stdafx.h"

#include <algorithm>

#include "InputLevel/Mesh/InputMesh.h"

namespace Pipeline
{
    // **************************************************************************************
    bool InputMesh::Vertex::operator==(const Vertex& other) {
        return
            other.PositionIndex == PositionIndex &&
            other.NormalIndex == NormalIndex &&
            other.TexCoordIndex == TexCoordIndex;
    }

    // **************************************************************************************
    InputMesh::InputMesh() {
        name_[0] = 0;
    }

    // **************************************************************************************
    void InputMesh::SetName(const char *name) {
        assert(name);
        strcpy_s(name_, name);
    }

    // **************************************************************************************
    const char *InputMesh::GetName() const {
        return name_;
    }

    // **************************************************************************************
    void InputMesh::SetPositions(std::vector<XMFLOAT4> &positions) {
        positions_.swap(positions);
    }

    // **************************************************************************************
    void InputMesh::SetTrianglesPositions(std::vector<uint16> &triangles) {
        trianglesPositions_.swap(triangles);
    }

    // **************************************************************************************
    void InputMesh::SetNormals(std::vector<XMFLOAT4> &normals) {
        normals_.swap(normals);
    }

    // **************************************************************************************
    void InputMesh::SetTexCoords(std::vector<XMFLOAT2> &texCoords) {
        texCoords_.swap(texCoords);
    }

    // **************************************************************************************
    void InputMesh::SetTrianglesTexCoords(std::vector<uint16> &triangles) {
        trianglesTexCoords_.swap(triangles);
    }

    // **************************************************************************************
    uint_t InputMesh::GetVerticesCount() const {
        return positions_.size();
    }

    // **************************************************************************************
    const std::vector<XMFLOAT4>& InputMesh::GetPositions() const {
        return positions_;
    }

    // **************************************************************************************
    const std::vector<uint16>& InputMesh::GetTrianglesPositions() const {
        return trianglesPositions_;
    }

    // **************************************************************************************
    const std::vector<XMFLOAT4>& InputMesh::GetNormals() const {
        return normals_;
    }

    // **************************************************************************************
    const std::vector<XMFLOAT2>& InputMesh::GetTexCoords() const {
        return texCoords_;
    }

    // **************************************************************************************
    const std::vector<uint16>& InputMesh::GetTrianglesTexCoords() const {
        return trianglesTexCoords_;
    }

    // **************************************************************************************
    void InputMesh::SetTransform(const XMFLOAT4X4& transform) {
        transform_ = transform;
    }

    // **************************************************************************************
    const XMFLOAT4X4& InputMesh::GetTransform() const {
        return transform_;
    }

    // **************************************************************************************
    const std::vector<InputMesh::Vertex>& InputMesh::GetVertices() const {
        return uniqueVertices_;
    }

    // **************************************************************************************
    const std::vector<uint16>& InputMesh::GetTrianglesVertices() const {
        return trianglesVertices_;
    }

    // **************************************************************************************
    void InputMesh::ComputeVertices() {
        const uint_t trianglesCount = trianglesPositions_.size() / 3;
        trianglesVertices_.resize(trianglesCount * 3);
        for (uint_t t = 0; t < trianglesCount; t++) {
            for (uint16 v = 0; v < 3; v++) {
                Vertex vv;
                vv.PositionIndex = vv.NormalIndex = trianglesPositions_[t * 3 + v];
                vv.TexCoordIndex = trianglesTexCoords_[t * 3 + v];

                std::vector<Vertex>::iterator it = std::find(uniqueVertices_.begin(), uniqueVertices_.end(), vv);
                if (it == uniqueVertices_.end()) {
                    uniqueVertices_.push_back(vv);
                    it = uniqueVertices_.end() - 1;
                }
                const uint_t vvIndex = it - uniqueVertices_.begin();
                trianglesVertices_[t * 3 + v] = static_cast<uint16>(vvIndex);
            }
        }
    }
}