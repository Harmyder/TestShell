#include "stdafx.h"

#include "UserLevel/Mesh/UserMesh.h"
#include "InputLevel/Mesh/InputMesh.h"

using namespace std;

namespace Pipeline
{
    // **************************************************************************************
    UserMesh::UserMesh(const InputMesh &mesh) :
        input_(mesh)
    {}

    // **************************************************************************************
    void UserMesh::SetName(const string name)
    {
        name_ = name;
    }

    // **************************************************************************************
    const XMFLOAT4X4& UserMesh::GetTransform() const
    {
        return transform_;
    }

    // **************************************************************************************
    void UserMesh::SetTransform(const XMFLOAT4X4& transform)
    {
        transform_ = transform;
    }

    // **************************************************************************************
    const MeshGeometry &UserMesh::GetGeometry() const
    {
        return meshGeometry_;
    }

    // **************************************************************************************
    MeshGeometry &UserMesh::GetNonConstGeometry()
    {
        return meshGeometry_;
    }

    // **************************************************************************************
    const InputMesh& UserMesh::GetInput() const
    {
        return input_;
    }
}
