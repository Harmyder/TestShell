// **************************************************************************************
// File: UserMeshFromInputMesh.h
// **************************************************************************************

#include "stdafx.h"

#include "UserLevel/UserMeshFromInputMesh.h"

#include "UserLevel/UserMesh.h"
#include "InputLevel/InputMesh.h"

#include "Common/Print/DebugPrint.h"

using namespace std;
using namespace Viewer;
using namespace Common;

namespace Pipeline
{
    UserMeshFromInputMesh::UserMeshFromInputMesh(UserMesh &mesh) :
        mesh_(mesh),
        inputMesh_(mesh.GetInput())
    {
    }

    void UserMeshFromInputMesh::Restore()
    {
        const XMFLOAT4X4 &src = inputMesh_.GetTransform();
        assert(src._14 == 0 && src._24 == 0 && src._34 == 0 && src._44 == 1);
        mesh_.SetTransform(Common::Matrix4(XMLoadFloat4x4(&src)));

        MeshGeometry &mg = mesh_.GetGeometryNonConst();

        const auto& positions = inputMesh_.GetPositions();
        const auto& normals = inputMesh_.GetNormals();
        const auto& texCoords = inputMesh_.GetTexCoords();

        const auto& visualVertices = inputMesh_.GetVisualVertices();
        mg.UniqueVertices = move(decltype(mg.UniqueVertices)(visualVertices.size()));
        for (uint32 i = 0; i < visualVertices.size(); ++i) {
            mg.UniqueVertices[i].Position = positions[visualVertices[i].PositionIndex];
            mg.UniqueVertices[i].Normal = normals[visualVertices[i].NormalIndex];
            mg.UniqueVertices[i].TexCoord = texCoords[visualVertices[i].TexCoordIndex];
        }
        mg.TrianglesVertices = inputMesh_.GetTrianglesVertices();

        const auto& trianglesPositions = inputMesh_.GetTrianglesPositions();
        mg.UniquePositions = move(vector<Vector3>(trianglesPositions.size(), Vector3(kInfinity)));
        for (uint32 i = 0; i < trianglesPositions.size(); ++i) {

        }
    }
}
