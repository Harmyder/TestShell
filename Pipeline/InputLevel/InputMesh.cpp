#include "stdafx.h"

#include <algorithm>
using namespace std;

#include "InputLevel/InputMesh.h"

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