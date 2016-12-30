// **************************************************************************************
// File: UserMeshFromInputMesh.h
// **************************************************************************************

#include "stdafx.h"

#include "UserLevel/UserMeshFromInputMesh.h"

#include "UserLevel/UserMesh.h"
#include "InputLevel/InputMesh.h"

using namespace std;
using namespace Viewer;

namespace Pipeline
{
    UserMeshFromInputMesh::UserMeshFromInputMesh(UserMesh &mesh) :
        mesh_(mesh),
        inputMesh_(mesh.GetInput())
    {
    }

    void UserMeshFromInputMesh::Restore()
    {
        mesh_.SetTransform(inputMesh_.GetTransform());

        MeshGeometry &mg = mesh_.GetNonConstGeometry();

        const auto XMFloat4to3 = [](XMFLOAT3 &p3, XMFLOAT4 p4) { p3.x = p4.x; p3.y = p4.y; p3.z = p4.z; };

        const auto& positions = inputMesh_.GetPositions();
        const auto& normals = inputMesh_.GetNormals();
        const auto& texCoords = inputMesh_.GetTexCoords();
        const auto& trianglePositions = inputMesh_.GetTrianglesPositions();
        const auto& triangleTexCoords = inputMesh_.GetTrianglesTexCoords();

        mg.Vertices = move(decltype(mg.Vertices)(trianglePositions.size()));
        for (uint32 i = 0; i < trianglePositions.size(); ++i) {
            XMFloat4to3(mg.Vertices[i].Position, positions[trianglePositions[i]]);
            XMFloat4to3(mg.Vertices[i].Normal, normals[trianglePositions[i]]);
            mg.Vertices[i].TexCoord = texCoords[triangleTexCoords[i]];
        }
    }
}
