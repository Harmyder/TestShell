#include "stdafx.h"

#include "UserLevel/UserMesh.h"
#include "InputLevel/InputMesh.h"

using namespace std;

namespace Pipeline
{
    // **************************************************************************************
    UserMesh::UserMesh(const InputMesh &mesh) :
        NamedByRefObject(mesh.GetName()),
        input_(mesh)
    {}

    // **************************************************************************************
    const XMFLOAT4X4& UserMesh::GetTransform() const {
        return transform_;
    }

    // **************************************************************************************
    void UserMesh::SetTransform(const XMFLOAT4X4& transform) {
        transform_ = transform;
    }

    // **************************************************************************************
    const MeshGeometry &UserMesh::GetGeometry() const {
        return meshGeometry_;
    }

    // **************************************************************************************
    MeshGeometry &UserMesh::GetGeometryNonConst() {
        return meshGeometry_;
    }

    // **************************************************************************************
    const InputMesh& UserMesh::GetInput() const {
        return input_;
    }
}
