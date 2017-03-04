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

        MeshGeometry &mg = mesh_.GetGeometryNonConst();

        const auto& positions = inputMesh_.GetPositions();
        const auto& normals = inputMesh_.GetNormals();
        const auto& texCoords = inputMesh_.GetTexCoords();
        const auto& trianglePositions = inputMesh_.GetTrianglesPositions();
        const auto& triangleTexCoords = inputMesh_.GetTrianglesTexCoords();

        mg.Vertices = move(decltype(mg.Vertices)(trianglePositions.size()));
        for (uint32 i = 0; i < trianglePositions.size(); ++i) {
            mg.Vertices[i].Position = positions[trianglePositions[i]];
            mg.Vertices[i].Normal = normals[trianglePositions[i]];
        }
        if (texCoords.size()) {
            for (uint32 i = 0; i < trianglePositions.size(); ++i) {
                mg.Vertices[i].TexCoord = texCoords[triangleTexCoords[i]];
            }
        }
    }
}
