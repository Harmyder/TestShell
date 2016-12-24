// **************************************************************************************
// File: UserMeshFromInputMesh.h
// **************************************************************************************

#include "stdafx.h"

#include "UserLevel/Mesh/Algorithm/UserMeshFromInputMesh.h"

#include "UserLevel/Mesh/UserMesh.h"
#include "InputLevel/Mesh/InputMesh.h"
#include "Viewer\Viewport.h"

using namespace std;
using namespace Viewer;

namespace Pipeline
{
    UserMeshFromInputMesh::UserMeshFromInputMesh(UserMesh &mesh) :
        mesh_(mesh),
        inputMesh_(mesh.GetInput())
    {
    }

    void UserMeshFromInputMesh::Restore(Viewport& viewport)
    {
        mesh_.SetName(inputMesh_.GetName());
        mesh_.SetTransform(inputMesh_.GetTransform());

        MeshGeometry &mg = mesh_.GetNonConstGeometry();

        const auto XMFloat4to3 = [](XMFLOAT3 &p3, XMFLOAT4 p4) { p3.x = p4.x; p3.y = p4.y; p3.z = p4.z; };

        const auto& positions = inputMesh_.GetPositions();
        const auto positionsCount = positions.size();
        mg.Vertices = move(decltype(mg.Vertices)(positionsCount));
        for (uint32 i = 0; i < positionsCount; ++i) {
            XMFloat4to3(mg.Vertices[i].Position, positions[i]);
            mg.Vertices[i].Color = XMFLOAT4(DirectX::Colors::White);
        }

        const auto& vertices = inputMesh_.GetVertices();
        const auto& triangleVertices = inputMesh_.GetTrianglesVertices();
        const auto verticesCount = vertices.size();
        const auto triangleVerticesCount = triangleVertices.size();
        mg.Indices = move(decltype(mg.Indices)(triangleVerticesCount));
        for (uint32 i = 0; i < triangleVerticesCount; ++i) {
            mg.Indices[i] = vertices[triangleVertices[i]].PositionIndex;
        }

        decltype(mg.Vertices) nonIndexedVertices;
        nonIndexedVertices.reserve(mg.Indices.size());
        for (uint_t i = 0; i < mg.Indices.size(); ++i) {
            nonIndexedVertices.push_back(mg.Vertices[mg.Indices[i]]);
        }
        mg.Vertices = move(nonIndexedVertices);

        auto rii = viewport.CreateRenderItem(mg.Vertices, mesh_.GetTransform());
        mesh_.SetRenderItemIndex(rii);
    }
}
