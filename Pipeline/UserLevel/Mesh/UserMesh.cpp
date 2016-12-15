#include "stdafx.h"

#include "UserLevel/Mesh/UserMesh.h"
#include "InputLevel/Mesh/InputMesh.h"

namespace Pipeline
{
    // **************************************************************************************
    UserMesh::UserMesh(const InputMesh &mesh) :
        m_Input(mesh)
    {
    }

    // **************************************************************************************
    void UserMesh::SetName(const char *name)
    {
        name_ = name;
    }

    // **************************************************************************************
    const char *UserMesh::GetName()
    {
        return name_;
    }

    // **************************************************************************************
    const XMFLOAT4X4& UserMesh::GetTransform() const
    {
        return m_Transform;
    }

    // **************************************************************************************
    void UserMesh::SetTransform(const XMFLOAT4X4& transform)
    {
        m_Transform = transform;
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
        return m_Input;
    }
}
